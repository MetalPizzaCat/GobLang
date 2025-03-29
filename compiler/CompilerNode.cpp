#include "CompilerNode.hpp"
#include "Compiler.hpp"
std::vector<uint8_t> GobLang::Compiler::TokenCompilerNode::getOperationGetBytes()
{
    return Compiler::generateGetByteCode(m_token);
}

std::vector<uint8_t> GobLang::Compiler::TokenCompilerNode::getOperationSetBytes()
{
    return Compiler::generateSetByteCode(m_token);
}
