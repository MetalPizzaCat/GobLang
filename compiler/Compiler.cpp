#include "Compiler.hpp"
#include "../execution/Machine.hpp"
#include <iostream>

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
            if (sepToken->getSeparator() == Separator::End)
            {
                dumpStack();
                continue;
            }
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
        if (dynamic_cast<IntToken *>(*it) != nullptr)
        {
            stack.push_back(new OperationCompilerNode(generateGetByteCode(*it)));
        }
        else if (dynamic_cast<IdToken *>(*it) != nullptr)
        {
            stack.push_back(new TokenCompilerNode(*it));
        }
        else if (OperatorToken *opToken = dynamic_cast<OperatorToken *>(*it); opToken != nullptr)
        {
            CompilerNode *a = stack[stack.size() - 2];
            CompilerNode *b = stack[stack.size() - 1];
            stack.pop_back();
            stack.pop_back();
            if (opToken->getOperator() == Operator::Assign)
            {
                appendByteCode(a->getOperationSetBytes());
                appendByteCode(b->getOperationGetBytes());

                m_byteCode.operations.push_back((uint8_t)SimpleLang::Operation::Set);
            }
            else
            {
                std::vector<uint8_t> opBytes;
                std::vector<uint8_t> aBytes = a->getOperationGetBytes();
                std::vector<uint8_t> bBytes = b->getOperationGetBytes();

                opBytes.insert(opBytes.end(), aBytes.begin(), aBytes.end());
                opBytes.insert(opBytes.end(), bBytes.begin(), bBytes.end());
                opBytes.push_back((uint8_t)opToken->getOperation());
                stack.push_back(new OperationCompilerNode(opBytes));
            }
            // since they are no longer on the stack they are not accessible outside of this block
            // leaving them undeleted will make them a memory leak
            delete a;
            delete b;
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
    return out;
}

void SimpleLang::Compiler::Compiler::appendByteCode(std::vector<uint8_t> const &code)
{
    for (uint8_t byte : code)
    {
        m_byteCode.operations.push_back(byte);
    }
}
