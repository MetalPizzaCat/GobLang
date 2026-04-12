#include "State.hpp"
#include "Instruction.hpp"
void GobLang::State::execute_closure(Closure const &closure)
{
    if (!closure.isLocal())
    {
        closure.getNativeFunction()(*this);
    }
    else
    {
        run_bytes(closure.getGobFunction());
    }
}
void GobLang::State::run_bytes(GobFunction const *func)
{
    size_t programCounter = 0;
    // TODO: Create variable block

    // TODO: Load arguments

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
            execute_closure(*f);
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

GobLang::GobFunction const *GobLang::State::createFunction(std::vector<uint8_t> const &bytecode, std::vector<std::string> strings, std::string const &name)
{
    m_objects.push_back(std::make_unique<GobLang::GobFunction>(bytecode, strings, name));
    return static_cast<GobLang::GobFunction *>(m_objects.back().get());
}

GobLang::Closure const *GobLang::State::createCppFunction(FunctionValue const &f)
{
    m_objects.push_back(std::make_unique<GobLang::Closure>(f));
    return static_cast<GobLang::Closure *>(m_objects.back().get());
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
