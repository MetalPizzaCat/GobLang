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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstUnsignedInt(uint32_t val)
{
    std::vector<uint8_t> res{(uint8_t)Operation::PushConstUnsignedInt};
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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstChar(char ch)
{
    return std::make_unique<GeneratedCodeGenValue>(
        std::vector<uint8_t>{
            (uint8_t)Operation::PushConstChar,
            (uint8_t)ch});
}

GobLang::Function const *GobLang::Codegen::Builder::addFunction(size_t nameId, std::vector<size_t> argIds)
{
    Function *func = new Function{.nameId = nameId};
    for (std::vector<size_t>::const_iterator it = argIds.begin(); it != argIds.end(); it++)
    {
        func->arguments.push_back(FunctionArgInfo{
            .nameId = *it,
        });
    }
    m_functions.push_back(std::unique_ptr<Function>(func));
    return m_functions.back().get();
}

void GobLang::Codegen::Builder::addType(
    std::string const &name,
    std::vector<std::string> const &fieldNames,
    size_t nameId,
    std::vector<size_t> fieldIds)
{
    Struct::Structure type = Struct::Structure{.name = name};
    for (std::string const &field : fieldNames)
    {
        type.fields.push_back(Struct::Field{.name = field});
    }
    m_types.push_back(std::make_unique<TypeCodeGenInfo>(nameId, std::move(fieldIds), std::move(type)));
}

bool GobLang::Codegen::Builder::hasLocalFunctionWithName(size_t nameId)
{
    return std::find_if(m_functions.begin(), m_functions.end(), [nameId](std::unique_ptr<GobLang::Function> const &func)
                        { return func->nameId == nameId; }) != m_functions.end();
}

bool GobLang::Codegen::Builder::hasTypeWithName(size_t nameId)
{
    return std::find_if(
               m_types.begin(),
               m_types.end(),
               [nameId](std::unique_ptr<TypeCodeGenInfo> const &type)
               {
                   return type->getNameId() == nameId;
               }) != m_types.end();
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

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createUnaryOperator(
    std::unique_ptr<CodeGenValue> right,
    Operator op)
{
    std::vector<uint8_t> bytes = right->getGetOperationBytes();
    switch (op)
    {
    case Operator::Sub:
        bytes.push_back((uint8_t)Operation::Negate);
        break;
    case Operator::BitNot:
        bytes.push_back((uint8_t)Operation::BitNot);
        break;

    case Operator::Not:
        bytes.push_back((uint8_t)Operation::Not);
        break;
    default:
        // unary '+' does nothing
        break;
    }
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createAssignment(
    std::unique_ptr<CodeGenValue> left,
    std::unique_ptr<CodeGenValue> right)
{

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
    std::vector<std::unique_ptr<GobLang::Function>>::const_iterator funcIt =
        std::find_if(m_functions.begin(), m_functions.end(), [nameId](std::unique_ptr<GobLang::Function> const &func)
                     { return func->nameId == nameId; });

    std::vector<uint8_t> bytes;

    for (std::vector<std::unique_ptr<CodeGenValue>>::const_iterator it = args.begin(); it != args.end(); it++)
    {
        std::vector<uint8_t> argBytes = (*it)->getGetOperationBytes();
        bytes.insert(bytes.begin(), argBytes.begin(), argBytes.end());
    }
    // call
    if (funcIt != m_functions.end())
    {
        bytes.push_back((uint8_t)Operation::GetLocalFunction);
        bytes.push_back((uint8_t)(funcIt - m_functions.begin()));
    }
    else
    {
        bytes.push_back((uint8_t)GobLang::Operation::PushConstString);
        bytes.push_back((uint8_t)nameId);
        bytes.push_back((uint8_t)GobLang::Operation::Get);
    }
    bytes.push_back((uint8_t)Operation::Call);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createConstructorCall(
    size_t typeId,
    std::vector<std::unique_ptr<CodeGenValue>> args)
{
    std::vector<std::unique_ptr<TypeCodeGenInfo>>::const_iterator typeIt =
        std::find_if(m_types.begin(), m_types.end(), [typeId](std::unique_ptr<TypeCodeGenInfo> const &type)
                     { return type->getNameId() == typeId; });

    std::vector<uint8_t> bytes;

    for (std::vector<std::unique_ptr<CodeGenValue>>::const_reverse_iterator it = args.rbegin(); it != args.rend(); it++)
    {
        std::vector<uint8_t> argBytes = (*it)->getGetOperationBytes();
        bytes.insert(bytes.begin(), argBytes.begin(), argBytes.end());
    }
    bytes.push_back((uint8_t)Operation::New);
    bytes.push_back((uint8_t)(typeIt - m_types.begin()));
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createCallFromValue(
    std::unique_ptr<CodeGenValue> value,
    std::vector<std::unique_ptr<CodeGenValue>> args)
{
    std::vector<uint8_t> bytes;

    for (std::vector<std::unique_ptr<CodeGenValue>>::const_reverse_iterator it = args.rbegin(); it != args.rend(); it++)
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

std::unique_ptr<GobLang::Codegen::FieldAccessCodeGenValue> GobLang::Codegen::Builder::createFieldAccess(
    std::unique_ptr<CodeGenValue> object,
    std::unique_ptr<CodeGenValue> field)
{
    return std::make_unique<FieldAccessCodeGenValue>(object->getGetOperationBytes(), field->getGetOperationBytes());
}

std::unique_ptr<GobLang::Codegen::VariableCodeGenValue> GobLang::Codegen::Builder::createVariableAccess(size_t nameId)
{
    size_t localId = getLocalVariableId(nameId);
    return std::make_unique<VariableCodeGenValue>(localId == -1 ? nameId : localId, localId != -1);
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::Builder::createLocalFunctionAccess(size_t nameId)
{
    std::vector<std::unique_ptr<GobLang::Function>>::const_iterator funcIt =
        std::find_if(
            m_functions.begin(),
            m_functions.end(),
            [nameId](std::unique_ptr<GobLang::Function> const &func)
            { return func->nameId == nameId; });
    return std::make_unique<GeneratedCodeGenValue>(std::vector<uint8_t>{
        (uint8_t)Operation::GetLocalFunction,
        (uint8_t)(funcIt - m_functions.begin())});
}

GobLang::Codegen::BlockContext *GobLang::Codegen::Builder::getCurrentBlock()
{
    if (m_blocks.empty())
    {
        return nullptr;
    }
    return m_blocks.back().get();
}

bool GobLang::Codegen::Builder::isCurrentlyInFunction()
{
    for (std::vector<std::unique_ptr<GobLang::Codegen::BlockContext>>::const_iterator it = m_blocks.begin(); it != m_blocks.end(); it++)
    {
        if ((*it)->isFunction())
        {
            return true;
        }
    }
    return false;
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

void GobLang::Codegen::Builder::pushBlockForFunction(Function const *func)
{
    std::vector<size_t> args;
    for (std::vector<FunctionArgInfo>::const_iterator it = func->arguments.begin(); it != func->arguments.end(); it++)
    {
        args.push_back(it->nameId);
    }
    m_blocks.push_back(std::make_unique<BlockContext>(func->nameId, args));
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

std::vector<Struct::Structure> GobLang::Codegen::Builder::getTypes() const
{
    std::vector<Struct::Structure> types;
    for (std::unique_ptr<TypeCodeGenInfo> const &type : m_types)
    {
        types.push_back(type->getType());
    }
    return types;
}
