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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstString(size_t strId)
{
    return std::make_unique<GeneratedCodeGenValue>(std::vector<uint8_t>{(uint8_t)Operation::PushConstString, (uint8_t)strId});
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstBool(bool val)
{
    return std::make_unique<GeneratedCodeGenValue>(std::vector<uint8_t>{
        (val ? (uint8_t)Operation::PushTrue : (uint8_t)Operation::PushFalse)});
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createOperation(
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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createAssignment(
    std::unique_ptr<CodeGenValue> left,
    std::unique_ptr<CodeGenValue> right,
    Operator op)
{

    std::vector<OperatorData>::const_iterator opIt = std::find_if(Operators.begin(), Operators.end(), [op](OperatorData const &opData)
                                                                  { return op == opData.op; });

    std::vector<uint8_t> bytes;
    std::vector<uint8_t> l = left->getSetOperationBytes();
    std::vector<uint8_t> r = right->getGetOperationBytes();
    bytes.insert(bytes.end(), r.begin(), r.end());
    bytes.insert(bytes.end(), l.begin(), l.end());

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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createCall(size_t nameId, std::vector<std::unique_ptr<CodeGenValue>> args)
{
    // TODO: add local functions
    std::vector<uint8_t> bytes;

    for (std::vector<std::unique_ptr<CodeGenValue>>::const_iterator it = args.begin(); it != args.end(); it++)
    {
        std::vector<uint8_t> argBytes = (*it)->getGetOperationBytes();
        bytes.insert(bytes.begin(), argBytes.begin(), argBytes.end());
    }
    // call
    bytes.push_back((uint8_t)GobLang::Operation::PushConstString);
    bytes.push_back((uint8_t)nameId);
    bytes.push_back((uint8_t)GobLang::Operation::Get);
    bytes.push_back((uint8_t)Operation::Call);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createCallFromValue(
    std::unique_ptr<CodeGenValue> value,
    std::vector<std::unique_ptr<CodeGenValue>> args)
{
    std::vector<uint8_t> bytes;

    for (std::vector<std::unique_ptr<CodeGenValue>>::const_iterator it = args.begin(); it != args.end(); it++)
    {
        std::vector<uint8_t> argBytes = (*it)->getGetOperationBytes();
        bytes.insert(bytes.begin(), argBytes.begin(), argBytes.end());
    }
    std::vector<uint8_t> valBytes = value->getGetOperationBytes();
    bytes.insert(bytes.end(), valBytes.begin(), valBytes.end());
    bytes.push_back((uint8_t)Operation::Call);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::ArrayAccessCodeGenValue> GobLang::Codegen::Builder::createArrayAccess(
    std::unique_ptr<CodeGenValue> array,
    std::unique_ptr<CodeGenValue> index)
{
    return std::make_unique<ArrayAccessCodeGenValue>(array->getGetOperationBytes(), index->getGetOperationBytes());
}

std::unique_ptr<GobLang::Codegen::VariableCodeGenValue> GobLang::Codegen::Builder::createVariableAccess(size_t nameId)
{
    size_t localId = getLocalVariableId(nameId);
    return std::make_unique<VariableCodeGenValue>(localId == -1 ? nameId : localId, localId != -1);
}

size_t GobLang::Codegen::Builder::getLocalVariableId(size_t nameId) const
{
    size_t offsetInFoundBlock = 0;
    bool found = false;
    for (std::vector<std::unique_ptr<BlockContext>>::const_reverse_iterator it = m_blocks.rbegin(); it != m_blocks.rend(); it++)
    {
        if (found)
        {
            offsetInFoundBlock += (*it)->getVariableCount();
        }
        else if ((*it)->getVariableLocalId(nameId) != -1)
        {
            found = true;
            offsetInFoundBlock = (*it)->getVariableLocalId(nameId);
        }
    }
    return found ? offsetInFoundBlock : -1;
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
    for (std::vector<std::unique_ptr<BlockContext>>::const_iterator it = m_blocks.begin(); it != m_blocks.end(); it++)
    {
        curr += (*it)->getVariableCount();
    }
    m_blocks.back()->insertVariable(nameId);
    return curr;
}
