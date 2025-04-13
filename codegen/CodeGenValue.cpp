#include "CodeGenValue.hpp"
#include "../execution/Operations.hpp"
#include "Builder.hpp"
#include "../execution/Value.hpp"

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

GobLang::Codegen::BlockContext::BlockContext(size_t funcId,
                                             std::vector<size_t> const &initialVariables) : m_variables(initialVariables),
                                                                                            m_funcId(funcId)
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
    if (m_variables.size() > 0)
    {
        m_bytes.push_back((uint8_t)Operation::ShrinkLocal);
        m_bytes.push_back((uint8_t)m_variables.size());
    }
}

GobLang::Codegen::BranchCodeGenValue::BranchCodeGenValue(std::vector<uint8_t> cond, std::vector<uint8_t> body)
    : m_condBytes(std::move(cond)), m_bodyBytes(std::move(body))
{
}

void GobLang::Codegen::BranchCodeGenValue::addJump(size_t offset)
{
    m_jumpAfter = offset;
}

void GobLang::Codegen::BranchCodeGenValue::addJumpBack(size_t offset)
{
    m_jumpAfter = offset;
}

std::vector<uint8_t> GobLang::Codegen::BranchCodeGenValue::getGetOperationBytes()
{
    std::vector<uint8_t> bytes = m_condBytes;

    bytes.insert(bytes.end(), m_bodyBytes.begin(), m_bodyBytes.end());
    if (m_jumpAfter != -1)
    {
        bytes.push_back(m_backwards ? (uint8_t)Operation::JumpBack : (uint8_t)Operation::Jump);
        std::vector<uint8_t> num = parseToBytes(m_jumpAfter);
        bytes.insert(bytes.end(), num.begin(), num.end());
    }
    return bytes;
}

void GobLang::Codegen::BranchCodeGenValue::setConditionJumpOffset(size_t offset)
{
    std::vector<uint8_t> num = parseToBytes(offset);
    std::copy(num.begin(), num.end(), m_condBytes.end() - sizeof(ProgramAddressType));
}

size_t GobLang::Codegen::BranchCodeGenValue::getBodySize()
{
    return m_bodyBytes.size() + ((m_jumpAfter != -1) ? (sizeof(ProgramAddressType) + 1) : 0);
}

size_t GobLang::Codegen::BranchCodeGenValue::getFullSize()
{
    return m_condBytes.size() + getBodySize();
}

size_t GobLang::Codegen::BranchCodeGenValue::getConditionSize()
{
    return m_condBytes.size();
}

GobLang::Codegen::ArrayAccessCodeGenValue::ArrayAccessCodeGenValue(std::vector<uint8_t> value,
                                                                   std::vector<uint8_t> address) : m_valueBytes(std::move(value)),
                                                                                                   m_addressBytes(std::move(address))
{
}

std::vector<uint8_t> GobLang::Codegen::ArrayAccessCodeGenValue::getGetOperationBytes()
{
    std::vector<uint8_t> bytes = m_addressBytes;
    bytes.insert(bytes.end(), m_valueBytes.begin(), m_valueBytes.end());
    bytes.push_back((uint8_t)Operation::GetArray);
    return bytes;
}

std::vector<uint8_t> GobLang::Codegen::ArrayAccessCodeGenValue::getSetOperationBytes()
{
    std::vector<uint8_t> bytes = m_addressBytes;
    bytes.insert(bytes.end(), m_valueBytes.begin(), m_valueBytes.end());
    bytes.push_back((uint8_t)Operation::SetArray);
    return bytes;
}

GobLang::Codegen::FunctionPrototypeCodeGenValue::FunctionPrototypeCodeGenValue(Function const *func) : m_func(func)
{
}

GobLang::Codegen::FunctionCodeGenValue::FunctionCodeGenValue(Function const *func,
                                                             std::unique_ptr<BlockContext> body) : m_body(std::move(body)),
                                                                                                   m_func(func)
{
}

std::vector<uint8_t> GobLang::Codegen::FunctionCodeGenValue::getGetOperationBytes()
{
    std::vector<uint8_t> body = m_body->getBytes();
    // in case we are in a situation where no return happened
    body.push_back((uint8_t)Operation::Return);
    return body;
}
