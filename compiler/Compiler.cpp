#include "Compiler.hpp"
#include "../execution/Machine.hpp"
#include "CompilerToken.hpp"
#include <iostream>
#include <deque>
#include <iterator>

void GobLang::Compiler::Compiler::generateByteCode()
{
    if (m_generator.getCode().empty())
    {
        return;
    }
    m_byteCode.ids = m_generator.getIds();
    m_byteCode.ints = m_generator.getInts();
    std::vector<CompilerNode *> stack;
    for (std::vector<Token *>::const_iterator it = m_generator.getCode().begin(); it != m_generator.getCode().end(); it++)
    {
        // check if there any marks pointing to this node
        bool isDestination = false;
        size_t destMark = 0;
        if (it != m_generator.getCode().begin())
        {
            if (JumpDestinationToken *dest = dynamic_cast<JumpDestinationToken *>(*(it - 1)); dest != nullptr)
            {
                destMark = dest->getId();
                isDestination = true;
            }
        }
        if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*it); sepTok != nullptr && sepTok->getSeparator() == Separator::End)
        {
            if (!stack.empty())
            {
                appendCompilerNode(*stack.rbegin(), true);
                delete *stack.rbegin();
                stack.pop_back();
            }
        }

        else if (BoolConstToken *boolToken = dynamic_cast<BoolConstToken *>(*it); boolToken != nullptr)
        {
            stack.push_back(new OperationCompilerNode(
                {(uint8_t)(boolToken->getValue() ? Operation::PushTrue : Operation::PushFalse)}, isDestination, destMark));
        }
        else if (dynamic_cast<IntToken *>(*it) != nullptr || dynamic_cast<StringToken *>(*it) != nullptr || dynamic_cast<CharToken *>(*it) != nullptr)
        {
            stack.push_back(new OperationCompilerNode(generateGetByteCode(*it), isDestination, destMark));
        }
        else if (dynamic_cast<IdToken *>(*it) != nullptr)
        {
            stack.push_back(new TokenCompilerNode(*it, isDestination, destMark));
        }
        else if (dynamic_cast<LocalVarToken *>(*it) != nullptr)
        {
            stack.push_back(new LocalVarTokenCompilerNode(*it, isDestination, destMark));
        }
        else if (JumpDestinationToken *destToken = dynamic_cast<JumpDestinationToken *>(*it); destToken != nullptr)
        {
            if (it + 1 != m_generator.getCode().end())
            {
                m_jumpDestinations[destToken->getId()] = m_byteCode.operations.size();
            }
        }
        else if (GotoToken *jmpToken = dynamic_cast<GotoToken *>(*it); jmpToken != nullptr)
        {
            std::vector<uint8_t> bytes;
            if (dynamic_cast<IfToken *>(jmpToken) != nullptr)
            {
                CompilerNode *cond = *stack.rbegin();
                stack.pop_back();
                // condition goes first and we don't care about anything else
                bytes = cond->getOperationGetBytes();
                bytes.push_back((uint8_t)Operation::JumpIfNot);
                delete cond;
            }
            else if (WhileToken *whileTok = dynamic_cast<WhileToken *>(jmpToken); whileTok != nullptr)
            {
                if (it + 1 != m_generator.getCode().end())
                {
                    m_jumpDestinations[whileTok->getReturnMark()] = m_byteCode.operations.size();
                }
                CompilerNode *cond = *stack.rbegin();
                stack.pop_back();
                // condition goes first and we don't care about anything else
                bytes = cond->getOperationGetBytes();
                bytes.push_back((uint8_t)Operation::JumpIfNot);
                delete cond;
            }
            else
            {
                bytes.push_back((uint8_t)Operation::Jump);
            }
            appendByteCode(bytes);
            addNewMarkReplacement(jmpToken->getMark(), m_byteCode.operations.size());
            for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
            {
                m_byteCode.operations.push_back(0x0);
            }
        }
        else if (FunctionCallToken *func = dynamic_cast<FunctionCallToken *>(*it); func != nullptr)
        {
            std::deque<CompilerNode *> nodes;
            for (int32_t i = 0; i < func->getArgCount(); i++)
            {
                nodes.push_front(*stack.rbegin());
                stack.pop_back();
            }
            std::vector<uint8_t> bytes;
            for (std::deque<CompilerNode *>::iterator it = nodes.begin(); it != nodes.end(); it++)
            {
                std::vector<uint8_t> temp = (*it)->getOperationGetBytes();
                bytes.insert(bytes.end(), temp.begin(), temp.end());
                // last time they are used, so we should delete them
                delete (*it);
            }
            CompilerNode *funcNode = *stack.rbegin();
            stack.pop_back();
            std::vector<uint8_t> fTemp = funcNode->getOperationGetBytes();
            bytes.insert(bytes.end(), fTemp.begin(), fTemp.end());
            delete funcNode;
            bytes.push_back((uint8_t)Operation::Call);
            stack.push_back(new OperationCompilerNode(bytes, isDestination, destMark));
        }
        else if (OperatorToken *opToken = dynamic_cast<OperatorToken *>(*it); opToken != nullptr)
        {
            if (opToken->isUnary())
            {
                // not only uses one argument
                CompilerNode *value = stack[stack.size() - 1];
                stack.pop_back();
                std::vector<uint8_t> opBytes = value->getOperationGetBytes();
                opBytes.push_back((uint8_t)opToken->getOperation());
                stack.push_back(new OperationCompilerNode(opBytes, isDestination, destMark));
                delete value;
                continue;
            }
            CompilerNode *setter = stack[stack.size() - 2];
            CompilerNode *valueToSet = stack[stack.size() - 1];
            stack.pop_back();
            stack.pop_back();
            if (opToken->getOperator() == Operator::Assign)
            {
                if (LocalVarTokenCompilerNode *localNode = dynamic_cast<LocalVarTokenCompilerNode *>(setter); localNode != nullptr)
                {
                    appendCompilerNode(valueToSet, true);
                    appendByteCode(localNode->getOperationSetBytes());
                }
                else
                {
                    appendCompilerNode(setter, false);
                    appendCompilerNode(valueToSet, true);
                    if (ArrayCompilerNode *arrNode = dynamic_cast<ArrayCompilerNode *>(setter); arrNode != nullptr)
                    {
                        m_byteCode.operations.push_back((uint8_t)GobLang::Operation::SetArray);
                    }
                    else
                    {
                        m_byteCode.operations.push_back((uint8_t)GobLang::Operation::Set);
                    }
                }
            }
            else
            {
                std::vector<uint8_t> opBytes;
                std::vector<uint8_t> aBytes = setter->getOperationGetBytes();
                std::vector<uint8_t> bBytes = valueToSet->getOperationGetBytes();

                opBytes.insert(opBytes.end(), aBytes.begin(), aBytes.end());
                opBytes.insert(opBytes.end(), bBytes.begin(), bBytes.end());
                opBytes.push_back((uint8_t)opToken->getOperation());
                stack.push_back(new OperationCompilerNode(opBytes, isDestination, destMark));
            }
            // since they are no longer on the stack they are not accessible outside of this block
            // leaving them undeleted will make them a memory leak
            delete setter;
            delete valueToSet;
        }
        else if (ArrayIndexToken *ait = dynamic_cast<ArrayIndexToken *>(*it); ait != nullptr)
        {
            CompilerNode *array = stack[stack.size() - 2];
            CompilerNode *index = stack[stack.size() - 1];
            stack.pop_back();
            stack.pop_back();

            stack.push_back(new ArrayCompilerNode(array, index, isDestination, destMark));
        }
        else if (LocalVarShrinkToken *shrinkTok = dynamic_cast<LocalVarShrinkToken *>(*it); shrinkTok != nullptr)
        {
            m_byteCode.operations.push_back((uint8_t)Operation::ShrinkLocal);
            m_byteCode.operations.push_back((uint8_t)shrinkTok->getAmount());
        }
    }
    for (std::vector<CompilerNode *>::iterator it = stack.begin(); it != stack.end(); it++)
    {
        appendCompilerNode(*it, true);
        delete (*it);
    }
    m_byteCode.operations.push_back((uint8_t)Operation::End);
    // since we can only be sure that we placed all marks at the end of the execution we can only do this here
    for (std::map<size_t, std::vector<size_t>>::iterator it = m_jumpMarks.begin(); it != m_jumpMarks.end(); it++)
    {
        if (std::map<size_t, size_t>::iterator jumpIt = m_jumpDestinations.find((*it).first); jumpIt != m_jumpDestinations.end())
        {
            _placeAddressForMark((*it).first, jumpIt->second, false);
        }
        else
        {
            // if we haven't recorded the mark then it means it's at the end
            _placeAddressForMark((*it).first, m_byteCode.operations.size() - 1, false);
        }
    }
}

std::vector<uint8_t> GobLang::Compiler::Compiler::generateGetByteCode(Token *token)
{
    std::vector<uint8_t> out;
    if (IntToken *intToken = dynamic_cast<IntToken *>(token); intToken != nullptr)
    {
        out.push_back((uint8_t)Operation::PushConstInt);
        out.push_back((uint8_t)intToken->getId());
    }
    if (StringToken *strToken = dynamic_cast<StringToken *>(token); strToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::PushConstString);
        out.push_back((uint8_t)strToken->getId());
    }
    else if (IdToken *idToken = dynamic_cast<IdToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::PushConstString);
        out.push_back((uint8_t)idToken->getId());
        out.push_back((uint8_t)GobLang::Operation::Get);
    }
    else if (ArrayIndexToken *arrToken = dynamic_cast<ArrayIndexToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::GetArray);
    }
    else if (LocalVarToken *localVarToken = dynamic_cast<LocalVarToken *>(token); localVarToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::GetLocal);
        out.push_back((uint8_t)localVarToken->getId());
    }
    else if (CharToken *chTok = dynamic_cast<CharToken *>(token); chTok != nullptr)
    {
        out.push_back((uint8_t)Operation::PushConstChar);
        out.push_back((uint8_t)chTok->getChar());
    }
    return out;
}

std::vector<uint8_t> GobLang::Compiler::Compiler::generateSetByteCode(Token *token)
{
    std::vector<uint8_t> out;
    if (IdToken *idToken = dynamic_cast<IdToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::PushConstString);
        out.push_back((uint8_t)idToken->getId());
    }
    else if (LocalVarToken *localVarToken = dynamic_cast<LocalVarToken *>(token); localVarToken != nullptr)
    {
        out.push_back((uint8_t)GobLang::Operation::SetLocal);
        out.push_back((uint8_t)localVarToken->getId());
    }
    return out;
}

void GobLang::Compiler::Compiler::appendCompilerNode(CompilerNode *node, bool getter)
{
    if (node->hasMark())
    {
        m_jumpDestinations[node->getMark()] = m_byteCode.operations.size() - 1;
    }
    std::vector<uint8_t> code = getter ? node->getOperationGetBytes() : node->getOperationSetBytes();
    m_byteCode.operations.insert(m_byteCode.operations.end(), code.begin(), code.end());
}

void GobLang::Compiler::Compiler::addNewMarkReplacement(size_t mark, size_t address)
{
    if (m_jumpMarks.count(mark) > 0)
    {
        m_jumpMarks[mark].push_back(address);
    }
    else
    {
        m_jumpMarks[mark] = {address};
    }
}

void GobLang::Compiler::Compiler::appendByteCode(std::vector<uint8_t> const &bytes)
{
    m_byteCode.operations.insert(m_byteCode.operations.end(), bytes.begin(), bytes.end());
}

void GobLang::Compiler::Compiler::_placeAddressForMark(size_t mark, size_t address, bool erase)
{
    for (std::vector<size_t>::iterator labelIt = m_jumpMarks[mark].begin();
         labelIt != m_jumpMarks[mark].end();
         labelIt++)
    {
        for (int32_t i = sizeof(size_t) - 1; i >= 0; i--)
        {
            size_t offset = (sizeof(uint8_t) * i) * 8;
            const size_t mask = 0xff;
            size_t num = address & (mask << offset);
            size_t numFixed = num >> offset;

            m_byteCode.operations[(*labelIt) + (sizeof(size_t) - 1 - i)] = (uint8_t)numFixed;
        }
    }
    if (erase)
    {
        m_jumpMarks.erase(mark);
    }
}
