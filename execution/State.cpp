#include "State.hpp"
#include "Instruction.hpp"
#include "../codegen/CodeGenerator.hpp"
#include <format>

/**
 * @brief Macro for performing operations on two values converting to correct types when needed. Macros aren't the best
 * But it's a lot better than a bunch of duplicated code
 *
 */
#define ARITH_OP(a, b, op)                                                                \
    if (std::holds_alternative<IntegerType>(a) && std::holds_alternative<IntegerType>(b)) \
    {                                                                                     \
        pushToStack(std::get<IntegerType>(a) op std::get<IntegerType>(b));                \
    }                                                                                     \
    if (std::holds_alternative<IntegerType>(a) && std::holds_alternative<NumberType>(b))  \
    {                                                                                     \
        pushToStack(std::get<IntegerType>(a) op(IntegerType) std::get<NumberType>(b));    \
    }                                                                                     \
    if (std::holds_alternative<NumberType>(a) && std::holds_alternative<NumberType>(b))   \
    {                                                                                     \
        pushToStack(std::get<NumberType>(a) op(IntegerType) std::get<NumberType>(b));     \
    }

void GobLang::State::executeClosure(Closure const &closure)
{
    if (!closure.isLocal())
    {
        closure.getNativeFunction()(*this);
    }
    else
    {
        runBytes(closure.getGobFunction());
    }
}
void GobLang::State::runBytes(GobFunction const *func)
{
    ProgramAddressType programCounter = 0;
    m_variables.emplace_back();

    for (ProgramAddressType i = 0; i < func->getArgumentCount(); i++)
    {
        setVariableValue(i, popFromStackOrError());
    }

    m_stack.emplace_back();

    std::vector<uint8_t> byteCode = func->getByteCode();

    while (programCounter < byteCode.size())
    {
        switch ((Instruction)func->getByteCode()[programCounter])
        {
        case Instruction::Add:
        {
            Value b = popFromStackOrError();
            Value a = popFromStackOrError();
            ARITH_OP(a, b, +);
            break;
        }
        case Instruction::Call:
        {
            Closure const *f = popFromStackAsType<Closure const *>("Expected callable object on stack");
            executeClosure(*f);
            break;
        }

        case Instruction::SetGlobal:
        {
            ProgramAddressType typeId = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(ProgramAddressType);
            setGlobalVariable(func->getConstantStringByidOrError(typeId), popFromStackOrError());

            break;
        }

        case Instruction::GetGlobal:
        {
            ProgramAddressType typeId = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(ProgramAddressType);
            pushToStack(getGlobalVariable(func->getConstantStringByidOrError(typeId)));

            break;
        }
        case Instruction::GetLocal:
        {
            ProgramAddressType typeId = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(ProgramAddressType);
            if (std::optional<Value> v = getVariableValue(typeId); v.has_value())
            {
                pushToStack(v.value());
            }
            else
            {
                throw Errors::ExecutionError("Unable to get value of local variable because no variable uses this id");
            }

            break;
        }
        case Instruction::SetLocal:
        {
            ProgramAddressType typeId = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(ProgramAddressType);
            setVariableValue(typeId, popFromStackOrError());
            break;
        }

        case Instruction::PushConstInt:
        {
            pushToStack(parseOperationConstant<IntegerType>(byteCode.begin() + (programCounter + 1), byteCode.end()));
            programCounter += sizeof(IntegerType);
            break;
        }

        case Instruction::PushConstFloat:
        {
            pushToStack(parseOperationConstant<NumberType>(byteCode.begin() + (programCounter + 1), byteCode.end()));
            programCounter += sizeof(NumberType);
            break;
        }
        case Instruction::PushConstChar:
        {
            pushToStack((char)byteCode[++programCounter]);
            break;
        }
        case Instruction::PushConstString:
        {
            ProgramAddressType typeId = parseOperationConstantAndAdvance<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end(), programCounter);
            pushToStack(createString(func->getConstantStringByidOrError(typeId)));
            break;
        }

        case Instruction::Equals:
        {
            Value a = popFromStackOrError();
            Value b = popFromStackOrError();
            pushToStack(ValueOperations::areEqual(a, b));
            break;
        }
        case Instruction::Less:
        {
            Value b = popFromStackOrError();
            Value a = popFromStackOrError();

            pushToStack(ValueOperations::less(a, b));
            break;
        }
        case Instruction::More:
        {
            break;
        }
        case Instruction::LessOrEq:
        {
            break;
        }
        case Instruction::MoreOrEq:
        {
            break;
        }
        case Instruction::NotEq:
        {
            break;
        }
        case Instruction::And:
        {
            break;
        }
        case Instruction::Or:
        {
            break;
        }
        case Instruction::Not:
        {
            break;
        }
        case Instruction::Jump:
        {
            ProgramAddressType addr = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += addr;
            continue;
        }
        case Instruction::JumpBack:
        {
            ProgramAddressType addr = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter -= addr;
            continue;
        }
        case Instruction::JumpIfNot:
        {

            if (Value cond = popFromStackOrError(); std::holds_alternative<bool>(cond))
            {
                ProgramAddressType addr = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
                if (!std::get<bool>(cond))
                {
                    programCounter += addr;
                    continue;
                }
                else
                {
                    programCounter += sizeof(ProgramAddressType);
                }
            }
            else
            {
                throw Errors::ExecutionError("Expected boolean value on stack");
            }
            break;
        }
        case Instruction::JumpIf:
        {

            if (Value cond = popFromStackOrError(); std::holds_alternative<bool>(cond))
            {
                ProgramAddressType addr = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
                if (std::get<bool>(cond))
                {
                    programCounter += addr;
                    continue;
                }
                else
                {
                    programCounter += sizeof(ProgramAddressType);
                }
            }
            else
            {
                throw Errors::ExecutionError("Expected boolean value on stack");
            }
            break;
        }
        case Instruction::ShrinkLocal:
        {
            ProgramAddressType size = parseOperationConstant<ProgramAddressType>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(ProgramAddressType);
            shrinkVariableFrameBy(size);
            break;
        }

        case Instruction::CreateArray:
        {
            programCounter++;
            int32_t arraySize = byteCode[programCounter];
            ArrayObject *array = createArray(arraySize);
            for (int32_t i = arraySize - 1; i >= 0; i--)
            {
                array->setItem(i, popFromStackOrError());
            }
            pushToStack(Value(array));
            break;
        }

        default:
            throw Errors::ExecutionError(std::format("Not implemented instruction with value {}", byteCode[programCounter]));
        }
        programCounter++;
    }

    m_stack.pop_back();

    // TODO: Pop variable block
}

std::optional<GobLang::Value> GobLang::State::getVariableValue(ProgramAddressType id) const
{
    if (m_variables.empty() || id >= m_variables.back().size())
    {
        return {};
    }
    return m_variables.back().at(id);
}

GobLang::Value GobLang::State::getGlobalVariable(std::string const &name) const
{
    if (m_globals.contains(name))
    {
        return m_globals.at(name);
    }
    return NilValue;
}

GobLang::Closure const *GobLang::State::loadString(std::string const &str)
{
    GobLang::Codegen::Parser comp(str);
    comp.parse();
    GobLang::Codegen::CodeGenerator gen(comp);
    return createClosure(gen.generate(*this), nullptr);
}

GobLang::Value GobLang::State::popFromStackOrError()
{
    if (m_stack.empty() || m_stack.back().empty())
    {
        throw Errors::RuntimeMemoryError("Can not pop from stack because stack is empty");
    }
    Value r = m_stack.back().back();
    m_stack.back().pop_back();
    return r;
}

void GobLang::State::setGlobalVariable(std::string const &name, Value const &v)
{
    m_globals[name] = v;
}

void GobLang::State::shrinkVariableFrameBy(size_t size)
{
    size_t i = 0;
    for (std::vector<Value>::reverse_iterator it = m_variables.back().rbegin(); it != m_variables.back().rend() && i < size; it++, i++)
    {
        ValueOperations::decreaseValueRefCount(*it);
    }
    // TODO: Consider not always shrinking the frame to save on performance?
    m_variables.back().resize(m_variables.back().size() - size);
}

void GobLang::State::setVariableValue(size_t id, Value const &val)
{
    if (m_variables.empty())
    {
        throw Errors::RuntimeMemoryError("No variable block is present");
    }
    std::vector<Value> &frame = m_variables.back();
    if (id >= frame.size())
    {
        // this should give us enough space
        // fill it with nil values
        frame.resize(id + 1, NilType());
    }
    else
    {
        // if we didn't have to resize that means we might have already used the slot
        // so to prepare to override we mark the object in it as unused
        ValueOperations::decreaseValueRefCount(frame[id]);
    }
    frame[id] = val;
    ValueOperations::increaseValueRefCount(val);
}

std::optional<GobLang::Value> GobLang::State::popFromStack()
{
    if (m_stack.empty() || m_stack.back().empty())
    {
        return {};
    }
    Value v = m_stack.back().back();
    m_stack.back().pop_back();
    return v;
}

GobLang::StringObject *GobLang::State::createString(std::string const &str)
{
    m_objects.push_back(std::make_unique<StringObject>(str));
    return static_cast<StringObject *>(m_objects.back().get());
}

GobLang::ArrayObject *GobLang::State::createArray(size_t size)
{
    m_objects.push_back(std::make_unique<ArrayObject>(size));
    return static_cast<ArrayObject *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createClosure(std::vector<uint8_t> const &bytecode, std::vector<std::string> strings, std::string const &name)
{
    m_objects.push_back(std::make_unique<GobLang::GobFunction>(bytecode, strings, name));
    m_objects.push_back(std::make_unique<Closure>(static_cast<GobFunction *>(m_objects.back().get()), nullptr));
    return static_cast<Closure const *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createClosure(FunctionValue const &f)
{
    m_objects.push_back(std::make_unique<GobLang::Closure>(f));
    return static_cast<GobLang::Closure *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createClosure(GobFunction *f, Object *owner)
{
    m_objects.push_back(std::make_unique<GobLang::Closure>(f, owner));
    return static_cast<GobLang::Closure *>(m_objects.back().get());
}

GobLang::GobFunction *GobLang::State::createFunction()
{
    m_objects.push_back(std::make_unique<GobLang::GobFunction>());
    return static_cast<GobLang::GobFunction *>(m_objects.back().get());
}

void GobLang::State::pushToStack(Value val)
{
    m_stack.back().push_back(val);
}

void GobLang::State::collectGarbage()
{
    for (int64_t i = m_objects.size() - 1; i >= 0; i--)
    {
        if (m_objects[i]->isDead())
        {
            m_objects.erase(m_objects.begin() + i);
        }
    }
}
