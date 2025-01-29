#include "Compiler.hpp"
#include "../execution/Machine.hpp"
#include "CompilerToken.hpp"
#include <iostream>
#include <deque>

void SimpleLang::Compiler::Compiler::compile()
{
    for (std::vector<Token *>::const_iterator it = m_parser.getTokens().begin(); it != m_parser.getTokens().end(); it++)
    {
        if (dynamic_cast<IdToken *>(*it) != nullptr || dynamic_cast<IntToken *>(*it) != nullptr)
        {
            m_code.push_back(*it);
            continue;
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
        if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*it); sepTok != nullptr && sepTok->getSeparator() == Separator::End)
        {
        }
        else if (dynamic_cast<IntToken *>(*it) != nullptr)
        {
            stack.push_back(new OperationCompilerNode(generateGetByteCode(*it)));
        }
        else if (dynamic_cast<IdToken *>(*it) != nullptr)
        {
            stack.push_back(new TokenCompilerNode(*it));
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
            stack.push_back(new OperationCompilerNode(bytes));
        }
        else if (OperatorToken *opToken = dynamic_cast<OperatorToken *>(*it); opToken != nullptr)
        {
            CompilerNode *setter = stack[stack.size() - 2];
            CompilerNode *valueToSet = stack[stack.size() - 1];
            stack.pop_back();
            stack.pop_back();
            if (opToken->getOperator() == Operator::Assign)
            {
                if (ArrayCompilerNode *arrNode = dynamic_cast<ArrayCompilerNode *>(setter); arrNode != nullptr)
                {
                    appendByteCode(setter->getOperationSetBytes());
                    appendByteCode(valueToSet->getOperationGetBytes());
                    m_byteCode.operations.push_back((uint8_t)SimpleLang::Operation::SetArray);
                }
                else
                {
                    appendByteCode(setter->getOperationSetBytes());
                    appendByteCode(valueToSet->getOperationGetBytes());
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
                stack.push_back(new OperationCompilerNode(opBytes));
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

            stack.push_back(new ArrayCompilerNode(array, index));
        }
    }
    for (std::vector<CompilerNode *>::iterator it = stack.begin(); it != stack.end(); it++)
    {
        appendByteCode((*it)->getOperationGetBytes());
        delete (*it);
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

void SimpleLang::Compiler::Compiler::appendByteCode(std::vector<uint8_t> const &code)
{
    m_byteCode.operations.insert(m_byteCode.operations.end(), code.begin(), code.end());
}

SimpleLang::Compiler::Compiler::~Compiler()
{
    for (size_t i = 0; i < m_compilerTokens.size(); i++)
    {
        delete m_compilerTokens[i];
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
        if (dynamic_cast<IdToken *>(*(it - 1)) != nullptr)
        {
            FunctionCallToken *token = new FunctionCallToken(sepToken->getRow(), sepToken->getColumn());
            m_compilerTokens.push_back(token);
            m_stack.push_back(token);
            m_functionCalls.push_back(token);
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
            }
        }
        break;
    }
}
