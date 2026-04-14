#include "State.hpp"
#include "Instruction.hpp"
#include "../codegen/CodeGenerator.hpp"
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
    size_t programCounter = 0;
    m_variables.emplace_back();

    for (size_t i = 0; i < func->getArgumentCount(); i++)
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

            break;
        }

        case Instruction::PushConstString:
        {
            size_t typeId = parseOperationConstant<int64_t>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(size_t);
            pushToStack(createString(func->getConstantStringByidOrError(typeId)));
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
            size_t typeId = parseOperationConstant<int64_t>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(size_t);
            setGlobalVariable(func->getConstantStringByidOrError(typeId), popFromStackOrError());

            break;
        }

        case Instruction::GetGlobal:
        {
            size_t typeId = parseOperationConstant<int64_t>(byteCode.begin() + (programCounter + 1), byteCode.end());
            programCounter += sizeof(size_t);
            pushToStack(getGlobalVariable(func->getConstantStringByidOrError(typeId)));

            break;
        }

        default:
            throw Errors::ExecutionError(std::string("Not implemented instruction with value ") + std::to_string(byteCode[programCounter]));
        }
        programCounter++;
    }

    m_stack.pop_back();

    // TODO: Pop variable block
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
        decreaseValueRefCount(frame[id]);
    }
    frame[id] = val;
    increaseValueRefCount(val);
}

void GobLang::State::setGlobalVariable(std::string const &name, Value const &v)
{
    m_globals[name] = v;
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
    return (GobLang::StringObject *)m_objects.back().get();
}

GobLang::Closure const *GobLang::State::createClosure(std::vector<uint8_t> const &bytecode, std::vector<std::string> strings, std::string const &name)
{
    m_objects.push_back(std::make_unique<GobLang::GobFunction>(bytecode, strings, name));
    m_objects.push_back(std::make_unique<Closure>(static_cast<GobFunction const *>(m_objects.back().get()), nullptr));
    return static_cast<Closure const *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createClosure(FunctionValue const &f)
{
    m_objects.push_back(std::make_unique<GobLang::Closure>(f));
    return static_cast<GobLang::Closure *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createClosure(GobFunction const *f, Object *owner)
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
