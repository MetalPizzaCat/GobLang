#include "CodeGenValue.hpp"
#include "../execution/Operations.hpp"
#include "Builder.hpp"

GobLang::Codegen::BlockCodeGenValue::BlockCodeGenValue(std::unique_ptr<BlockContext> block) : m_block(std::move(block))
{
}

GobLang::Codegen::VariableCodeGenValue::VariableCodeGenValue(size_t nameId, bool local) : m_id(nameId), m_local(local)
{
}

std::vector<uint8_t> GobLang::Codegen::VariableCodeGenValue::getGetOperationBytes()
{
    if (!m_local)
    {
        return {(uint8_t)Operation::PushConstString,
                (uint8_t)m_id,
                (uint8_t)Operation::Get};
    }
    return {(uint8_t)Operation::GetLocal, (uint8_t)m_id};
}

std::vector<uint8_t> GobLang::Codegen::VariableCodeGenValue::getSetOperationBytes()
{
    if (!m_local)
    {
        return {(uint8_t)Operation::PushConstString,
                (uint8_t)m_id,
                (uint8_t)Operation::Set};
    }
    return {(uint8_t)Operation::SetLocal, (uint8_t)m_id};
}

GobLang::Codegen::GeneratedCodeGenValue::GeneratedCodeGenValue(std::vector<uint8_t> val) : m_bytes(std::move(val))
{
}

void GobLang::Codegen::BlockContext::insertVariable(size_t name)
{
    m_variables.push_back(name);
}

size_t GobLang::Codegen::BlockContext::getVariableLocalId(size_t nameId)
{
    std::vector<size_t>::iterator it = std::find(m_variables.begin(), m_variables.end(), nameId);
    if (it == m_variables.end())
    {
        return -1;
    }
    else
    {
        return std::find(m_variables.begin(), m_variables.end(), nameId) - m_variables.begin();
    }
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