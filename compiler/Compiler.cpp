#include "Compiler.hpp"
#include "../execution/Machine.hpp"
#include "CompilerToken.hpp"
#include <iostream>
#include <deque>

void SimpleLang::Compiler::Compiler::compile()
{
    for (std::vector<Token *>::const_iterator it = m_parser.getTokens().begin(); it != m_parser.getTokens().end(); it++)
    {
        if (dynamic_cast<IdToken *>(*it) != nullptr || dynamic_cast<IntToken *>(*it) != nullptr || dynamic_cast<StringToken *>(*it) != nullptr)
        {
            m_code.push_back(*it);
            continue;
        }
        else if (KeywordToken *keyTok = dynamic_cast<KeywordToken *>(*it); keyTok != nullptr)
        {
            _compileKeywords(keyTok, it);
        }
        else if (SeparatorToken *sepToken = dynamic_cast<SeparatorToken *>(*it); sepToken != nullptr)
        {
            _compileSeparators(sepToken, it);
        }
        else if (dynamic_cast<OperatorToken *>(*it) != nullptr)
        {
            if ((*it)->getPriority() > getTopStackPriority())
            {
                m_stack.push_back(*it);
                continue;
            }
            while (!m_stack.empty() && getTopStackPriority() >= (*it)->getPriority())
            {
                m_code.push_back(popStack());
            }
            m_stack.push_back(*it);
        }
    }
    dumpStack();
}

void SimpleLang::Compiler::Compiler::generateByteCode()
{
    if (m_code.empty())
    {
        return;
    }
    m_byteCode.ids = m_parser.getIds();
    m_byteCode.ints = m_parser.getInts();
    std::vector<CompilerNode *> stack;
    for (std::vector<Token *>::iterator it = m_code.begin(); it != m_code.end(); it++)
    {
        // check if there any marks pointing to this node
        bool isDestination = false;
        size_t destMark = 0;
        if (it != m_code.begin())
        {
            if (JumpDestinationToken *dest = dynamic_cast<JumpDestinationToken *>(*(it - 1)); dest != nullptr)
            {
                destMark = dest->getId();
                isDestination = true;
                std::cout << "Makred " << (*it)->toString() << std::endl;
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
        else if (dynamic_cast<IntToken *>(*it) != nullptr || dynamic_cast<StringToken *>(*it) != nullptr)
        {
            stack.push_back(new OperationCompilerNode(generateGetByteCode(*it), isDestination, destMark));
        }
        else if (dynamic_cast<IdToken *>(*it) != nullptr)
        {
            stack.push_back(new TokenCompilerNode(*it, isDestination, destMark));
        }
        else if (JumpDestinationToken *destToken = dynamic_cast<JumpDestinationToken *>(*it); destToken != nullptr)
        {
            if (it + 1 != m_code.end())
            {
                m_jumpDestinations[destToken->getId()] = m_byteCode.operations.size();
            }
        }
        else if (GotoToken *jmpToken = dynamic_cast<GotoToken *>(*it); jmpToken != nullptr)
        {
            CompilerNode *cond = *stack.rbegin();
            stack.pop_back();
            // condition goes first and we don't care about anything else
            std::vector<uint8_t> bytes = cond->getOperationGetBytes();
            if (dynamic_cast<IfToken *>(jmpToken) != nullptr)
            {
                bytes.push_back((uint8_t)Operation::JumpIfNot);
            }
            else
            {
                bytes.push_back((uint8_t)Operation::Jump);
            }
            m_byteCode.operations.insert(m_byteCode.operations.end(), bytes.begin(), bytes.end());
            addNewMarkReplacement(jmpToken->getMark(), m_byteCode.operations.size());
            for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
            {
                m_byteCode.operations.push_back(0x0);
            }
            delete cond;
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
            CompilerNode *setter = stack[stack.size() - 2];
            CompilerNode *valueToSet = stack[stack.size() - 1];
            stack.pop_back();
            stack.pop_back();
            if (opToken->getOperator() == Operator::Assign)
            {
                appendCompilerNode(setter, false);
                appendCompilerNode(valueToSet, true);
                if (ArrayCompilerNode *arrNode = dynamic_cast<ArrayCompilerNode *>(setter); arrNode != nullptr)
                {
                    m_byteCode.operations.push_back((uint8_t)SimpleLang::Operation::SetArray);
                }
                else
                {
                    m_byteCode.operations.push_back((uint8_t)SimpleLang::Operation::Set);
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

void SimpleLang::Compiler::Compiler::dumpStack()
{
    // dump the remaining stack
    for (std::vector<Token *>::const_reverse_iterator it = m_stack.rbegin(); it != m_stack.rend(); it++)
    {
        m_code.push_back(*it);
    }
    m_stack.clear();
}

void SimpleLang::Compiler::Compiler::dumpStackWhile(std::function<bool(Token *)> const &cond)
{
    for (int32_t i = m_stack.size() - 1; i >= 0 && cond(m_stack[i]); i--)
    {
        m_code.push_back(m_stack[i]);
        m_stack.pop_back();
    }
}

int32_t SimpleLang::Compiler::Compiler::getTopStackPriority()
{
    if (m_stack.empty())
    {
        return -1;
    }
    return m_stack[m_stack.size() - 1]->getPriority();
}

SimpleLang::Compiler::Token *SimpleLang::Compiler::Compiler::popStack()
{
    if (m_stack.empty())
    {
        return nullptr;
    }
    Token *tkn = m_stack[m_stack.size() - 1];
    m_stack.pop_back();
    return tkn;
}

void SimpleLang::Compiler::Compiler::printCode()
{
    for (std::vector<Token *>::iterator it = m_code.begin(); it != m_code.end(); it++)
    {
        std::cout << (*it)->toString() << " ";
    }
    std::cout << std::endl;
}

std::vector<uint8_t> SimpleLang::Compiler::Compiler::generateGetByteCode(Token *token)
{
    std::vector<uint8_t> out;
    if (IntToken *intToken = dynamic_cast<IntToken *>(token); intToken != nullptr)
    {
        out.push_back((uint8_t)Operation::PushConstInt);
        out.push_back((uint8_t)intToken->getId());
    }
    if (StringToken *strToken = dynamic_cast<StringToken *>(token); strToken != nullptr)
    {
        out.push_back((uint8_t)SimpleLang::Operation::PushConstString);
        out.push_back((uint8_t)strToken->getId());
    }
    else if (IdToken *idToken = dynamic_cast<IdToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)SimpleLang::Operation::PushConstString);
        out.push_back((uint8_t)idToken->getId());
        out.push_back((uint8_t)SimpleLang::Operation::Get);
    }
    else if (ArrayIndexToken *arrToken = dynamic_cast<ArrayIndexToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)SimpleLang::Operation::GetArray);
    }
    return out;
}

std::vector<uint8_t> SimpleLang::Compiler::Compiler::generateSetByteCode(Token *token)
{
    std::vector<uint8_t> out;
    if (IdToken *idToken = dynamic_cast<IdToken *>(token); idToken != nullptr)
    {
        out.push_back((uint8_t)SimpleLang::Operation::PushConstString);
        out.push_back((uint8_t)idToken->getId());
    }
    // else if (ArrayIndexToken *arrToken = dynamic_cast<ArrayIndexToken *>(token); idToken != nullptr)
    // {
    //     out.push_back((uint8_t)SimpleLang::Operation::SetArray);
    // }
    return out;
}

void SimpleLang::Compiler::Compiler::appendCompilerNode(CompilerNode *node, bool getter)
{
    if (node->hasMark())
    {
        m_jumpDestinations[node->getMark()] = m_byteCode.operations.size() - 1;
    }
    std::vector<uint8_t> code = getter ? node->getOperationGetBytes() : node->getOperationSetBytes();
    m_byteCode.operations.insert(m_byteCode.operations.end(), code.begin(), code.end());
}

void SimpleLang::Compiler::Compiler::addNewMarkReplacement(size_t mark, size_t address)
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

size_t SimpleLang::Compiler::Compiler::getMarkCounterAndAdvance()
{
    return m_markCounter++;
}

SimpleLang::Compiler::Compiler::~Compiler()
{
    for (size_t i = 0; i < m_compilerTokens.size(); i++)
    {
        delete m_compilerTokens[i];
    }
}

void SimpleLang::Compiler::Compiler::_placeAddressForMark(size_t mark, size_t address, bool erase)
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

void SimpleLang::Compiler::Compiler::_compileSeparators(SeparatorToken *sepToken, std::vector<Token *>::const_iterator const &it)
{

    switch (sepToken->getSeparator())
    {
    case Separator::End:
        dumpStack();
        m_code.push_back(sepToken);
        break;
    case Separator::BracketOpen:
        if (!m_isInConditionHead)
        {
            if (dynamic_cast<IdToken *>(*(it - 1)) != nullptr)
            {
                FunctionCallToken *token = new FunctionCallToken(sepToken->getRow(), sepToken->getColumn());
                m_compilerTokens.push_back(token);
                m_stack.push_back(token);
                m_functionCalls.push_back(token);
            }
        }
        break;
    case Separator::Comma:
        if (!m_functionCalls.empty())
        {
            dumpStackWhile([](Token *t)
                           { return dynamic_cast<FunctionCallToken *>(t) == nullptr; });
            (*m_functionCalls.rbegin())->increaseArgCount();
        }
        break;
    case Separator::ArrayOpen:
    {
        ArrayIndexToken *token = new ArrayIndexToken(sepToken->getRow(), sepToken->getColumn());
        m_compilerTokens.push_back(token);
        m_stack.push_back(token);
    }
    break;
    case Separator::ArrayClose:

        if (m_stack.empty())
        {
            throw ParsingError(sepToken->getRow(), sepToken->getColumn(), "Encountered ')' without previous '('");
        }
        else
        {
            Token *arrayPopToken = nullptr;
            do
            {
                arrayPopToken = *(m_stack.rbegin());
                m_code.push_back(arrayPopToken);
                m_stack.pop_back();
            } while (!m_stack.empty() && dynamic_cast<ArrayIndexToken *>(arrayPopToken) == nullptr);
        }
        break;

    case Separator::BracketClose:
        while (!m_stack.empty())
        {
            Token *t = *(m_stack.rbegin());
            m_stack.pop_back();
            if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(t); sepTok != nullptr && sepTok->getSeparator() == Separator::BracketOpen)
            {
                break;
            }
            else if (FunctionCallToken *funcTok = dynamic_cast<FunctionCallToken *>(t); funcTok != nullptr)
            {
                SeparatorToken *prev = dynamic_cast<SeparatorToken *>(*(it - 1));
                if (!(prev != nullptr && (prev->getSeparator() == Separator::BracketOpen || prev->getSeparator() == Separator::Comma)))
                {
                    (*m_functionCalls.rbegin())->increaseArgCount();
                }
                m_code.push_back(*m_functionCalls.rbegin());
                m_functionCalls.pop_back();
            }
            else
            {
                m_code.push_back(t);
                if (dynamic_cast<IfToken *>(t) != nullptr)
                {
                    m_isInConditionHead = false;
                }
            }
        }
        break;
    case Separator::BlockOpen:
        break;
    case Separator::BlockClose:
        dumpStack();
        if (!m_jumps.empty())
        {
            GotoToken *jump = *m_jumps.rbegin();
            m_jumps.pop_back();
            if (IfToken *ifToken = dynamic_cast<IfToken *>(jump); ifToken != nullptr && ifToken->isElif())
            {
                if (m_jumps.empty())
                {
                    throw ParsingError(ifToken->getRow(), ifToken->getColumn(), "Invalid elif configuration, jump token was not generated");
                }
                GotoToken *elifPair = *m_jumps.rbegin();
                elifPair->setMark(getMarkCounterAndAdvance());
                JumpDestinationToken *dest = new JumpDestinationToken(sepToken->getRow(), sepToken->getColumn(), elifPair->getMark());
                m_compilerTokens.push_back(dest);
                m_code.push_back(dest);
            }
            jump->setMark(getMarkCounterAndAdvance());
            JumpDestinationToken *dest = new JumpDestinationToken(sepToken->getRow(), sepToken->getColumn(), jump->getMark());
            if (_isElseChainToken(it + 1) || _isElifChainToken(it + 1))
            {
                GotoToken *elseJump = new GotoToken(sepToken->getRow(), sepToken->getColumn());
                m_jumps.push_back(elseJump);
                m_compilerTokens.push_back(elseJump);
                m_code.push_back(elseJump);
            }

            m_compilerTokens.push_back(dest);
            m_code.push_back(dest);
        }
        break;
    }
}

void SimpleLang::Compiler::Compiler::_compileKeywords(KeywordToken *keyToken, std::vector<Token *>::const_iterator const &it)
{
    switch (keyToken->getKeyword())
    {
    case Keyword::Elif:
    case Keyword::If:
    {
        if (it + 1 == m_parser.getTokens().end() || dynamic_cast<SeparatorToken *>(*(it + 1)) == nullptr)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Missing condition for 'if' construct");
        }
        else if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*(it + 1)); sepTok != nullptr && sepTok->getSeparator() != Separator::BracketOpen)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Missing condition for 'if' construct");
        }
        IfToken *ifTok = new IfToken(keyToken->getRow(), keyToken->getColumn(), keyToken->getKeyword() == Keyword::Elif);
        m_stack.push_back(ifTok);
        m_jumps.push_back(ifTok);
        m_compilerTokens.push_back(ifTok);
        m_isInConditionHead = true;
    }
    break;
    case Keyword::Else:
        if (it + 1 == m_parser.getTokens().end() || dynamic_cast<SeparatorToken *>(*(it + 1)) == nullptr)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Else construct is missing body block");
        }
        else if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*(it + 1)); sepTok != nullptr && sepTok->getSeparator() != Separator::BlockOpen)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Else keyword must be followed by a code block");
        }

        break;
    default:
        throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Invalid keyword encountered");
        break;
    }
}

bool SimpleLang::Compiler::Compiler::_isElseChainToken(std::vector<Token *>::const_iterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return false;
    }
    KeywordToken *key = dynamic_cast<KeywordToken *>(*it);
    return key != nullptr && key->getKeyword() == Keyword::Else;
}

bool SimpleLang::Compiler::Compiler::_isElifChainToken(std::vector<Token *>::const_iterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return false;
    }
    KeywordToken *key = dynamic_cast<KeywordToken *>(*it);
    return key != nullptr && key->getKeyword() == Keyword::Elif;
}
