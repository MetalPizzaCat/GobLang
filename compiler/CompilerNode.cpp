#include "CompilerNode.hpp"
#include "Compiler.hpp"
std::vector<uint8_t> SimpleLang::Compiler::TokenCompilerNode::getOperationGetBytes()
{
    return Compiler::generateGetByteCode(m_token);
}

std::vector<uint8_t> SimpleLang::Compiler::TokenCompilerNode::getOperationSetBytes()
{
    return Compiler::generateSetByteCode(m_token);
}

void SimpleLang::Compiler::CompilerNode::setMark(size_t mark)
{
    m_hasMark = true;
    m_attachedMark = mark;
}
