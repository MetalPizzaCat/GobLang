#include "Builder.hpp"
#include "../execution/Operations.hpp"
#include "Lexems.hpp"
#include <algorithm>

using namespace GobLang;
std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstFloat(float val)
{

    std::vector<uint8_t> res{(uint8_t)Operation::PushConstFloat};
    std::vector<uint8_t> num = parseToBytes(val);
    res.insert(res.end(), num.begin(), num.end());

    return std::make_unique<GeneratedCodeGenValue>(std::move(res));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstInt(int32_t val)
{
    std::vector<uint8_t> res{(uint8_t)Operation::PushConstInt};
    std::vector<uint8_t> num = parseToBytes(val);
    res.insert(res.end(), num.begin(), num.end());

    return std::make_unique<GeneratedCodeGenValue>(std::move(res));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createFloatOperation(
    std::unique_ptr<CodeGenValue> left,
    std::unique_ptr<CodeGenValue> right,
    Operator op)
{
    std::vector<OperatorData>::const_iterator opIt = std::find_if(Operators.begin(), Operators.end(), [op](OperatorData const &opData)
                                                                  { return op == opData.op; });

    std::vector<uint8_t> bytes;
    std::vector<uint8_t> l = left->getGetOperationBytes();
    std::vector<uint8_t> r = right->getGetOperationBytes();
    bytes.insert(bytes.end(), l.begin(), l.end());
    bytes.insert(bytes.end(), r.begin(), r.end());
    bytes.push_back((uint8_t)opIt->operation);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createVariableInit(size_t id, std::unique_ptr<CodeGenValue> init)
{
    std::vector<uint8_t> bytes;
    std::vector<uint8_t> initBytes = init->getGetOperationBytes();
    bytes.insert(bytes.end(), initBytes.begin(), initBytes.end());
    bytes.push_back((uint8_t)Operation::SetLocal);
    bytes.push_back((uint8_t)id);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

void GobLang::Codegen::Builder::pushEmptyBlock()
{
    m_blocks.push_back(std::make_unique<BlockContext>());
}

std::unique_ptr<GobLang::Codegen::BlockContext> GobLang::Codegen::Builder::popBlock()
{
    std::unique_ptr<BlockContext> b = std::move(m_blocks.back());
    m_blocks.pop_back();
    return b;
}

size_t GobLang::Codegen::Builder::insertVariable(size_t nameId)
{
    size_t curr = 0;
    for (std::vector<std::unique_ptr<BlockContext>>::const_reverse_iterator it = m_blocks.rbegin(); it != m_blocks.rend(); it++)
    {
        curr += (*it)->getVariableCount();
    }
    m_blocks.back()->insertVariable(nameId);
    return curr;
}

GobLang::Codegen::GeneratedCodeGenValue::GeneratedCodeGenValue(std::vector<uint8_t> val) : m_bytes(std::move(val))
{
}

size_t GobLang::Codegen::BlockContext::getVariableLocalId(size_t nameId)
{
    return std::find(m_variables.begin(), m_variables.end(), nameId) - m_variables.begin();
}

bool GobLang::Codegen::BlockContext::hasVariableWithNameId(size_t nameId)
{
    return std::find(m_variables.begin(), m_variables.end(), nameId) != m_variables.end();
}

void GobLang::Codegen::BlockContext::insert(std::vector<uint8_t> const &bytes)
{
    m_bytes.insert(m_bytes.end(), bytes.begin(), bytes.end());
}

void GobLang::Codegen::BlockContext::appendMemoryClear()
{
    m_bytes.push_back((uint8_t)Operation::ShrinkLocal);
    m_bytes.push_back((uint8_t)m_variables.size());
}

GobLang::Codegen::BlockCodeGenValue::BlockCodeGenValue(std::unique_ptr<BlockContext> block) : m_block(std::move(block))
{
}
