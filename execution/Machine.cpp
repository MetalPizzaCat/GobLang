#include "Machine.hpp"
#include "FunctionRef.hpp"
#include <iostream>
#include <vector>
GobLang::Machine::Machine(Codegen::ByteCode const &code)
{
    m_constStrings = code.ids;
    m_operations = code.operations;
    m_functions = code.functions;
    for (Struct::Structure const &structure : code.structures)
    {
        Structure *str = new Structure();
        *str = structure;
        m_structures.push_back(std::unique_ptr<Structure>(str));
    }
}
void GobLang::Machine::addFunction(FunctionValue const &func, std::string const &name)

{
    m_globals[name] = MemoryValue{.type = Type::NativeFunction, .value = func};
}
void GobLang::Machine::step()
{
    if (m_programCounter >= m_operations.size())
    {
        return;
    }
    switch ((Operation)m_operations[m_programCounter])
    {
    case Operation::Add:
        _add();
        break;
    case Operation::Sub:
        _sub();
        break;
    case Operation::Mul:
        _mul();
        break;
    case Operation::Div:
        _div();
        break;
    case Operation::Modulo:
        _mod();
        break;
    case Operation::Call:
        _call();
        break;
    case Operation::GetLocalFunction:
        _getLocalFunc();
        break;
    case Operation::Set:
        _set();
        collectGarbage();
        break;
    case Operation::Get:
        _get();
        break;
    case Operation::GetLocal:
        _getLocal();
        break;
    case Operation::BitAnd:
        _bitAnd();
        break;
    case Operation::BitOr:
        _bitOr();
        break;
    case Operation::BitXor:
        _bitXor();
        break;
    case Operation::BitNot:
        _bitNot();
        break;
    case Operation::ShiftLeft:
        _shiftLeft();
        break;
    case Operation::ShiftRight:
        _shiftRight();
        break;
    case Operation::SetLocal:
        _setLocal();
        collectGarbage();
        break;
    case Operation::PushConstInt:
        _pushConstInt();
        break;
    case Operation::PushConstUnsignedInt:
        _pushConstUnsignedInt();
        break;
    case Operation::PushConstFloat:
        _pushConstFloat();
        break;
    case Operation::PushConstChar:
        _pushConstChar();
        break;
    case Operation::PushConstString:
        _pushConstString();
        break;
    case Operation::GetArray:
        _getArray();
        break;
    case Operation::SetArray:
        _setArray();
        collectGarbage();
        break;
    case Operation::GetField:
        _getField();
        break;
    case Operation::SetField:
        _setField();
        collectGarbage();
        break;
    case Operation::CallMethod:
        _callMethod();
        break;
    case Operation::Jump:
        _jump();
        return; // this uses return because we want to avoid advancing the counter after jmp
    case Operation::JumpBack:
        _jumpBack();
        return;
    case Operation::JumpIfNot:
        _jumpIf();
        return;
    case Operation::PushTrue:
        pushToStack(MemoryValue{.type = Type::Bool, .value = true});
        break;
    case Operation::PushFalse:
        pushToStack(MemoryValue{.type = Type::Bool, .value = false});
        break;
    case Operation::PushNull:
        _pushConstNull();
        break;
    case Operation::Equals:
        _eq();
        break;
    case Operation::NotEq:
        _neq();
        break;
    case Operation::Less:
        _less();
        break;
    case Operation::More:
        _more();
        break;
    case Operation::Not:
        _not();
        break;
    case Operation::And:
        _and();
        break;
    case Operation::Or:
        _or();
        break;
    case Operation::LessOrEq:
        _lessOrEq();
        break;
    case Operation::MoreOrEq:
        _moreOrEq();
        break;
    case Operation::Negate:
        _negate();
        break;
    case Operation::ShrinkLocal:
        _shrink();
        collectGarbage();
        break;
    case Operation::Return:
        _return();
        collectGarbage();
        break;
    case Operation::ReturnValue:
        _returnWithValue();
        break;
    case Operation::CreateArray:
        _createArray();
        break;
    case Operation::New:
        _new();
        break;
    case Operation::End:
        m_forcedEnd = true;
        break;
    default:
        std::cerr << "Invalid op code: " << (int32_t)m_operations[m_programCounter] << " at " << std::hex << m_programCounter << std::dec << std::endl;
        break;
    }
    m_programCounter++;
}

void GobLang::Machine::printGlobalsInfo()
{
    for (std::map<std::string, MemoryValue>::iterator it = m_globals.begin(); it != m_globals.end(); it++)
    {
        std::cout << it->first << "(" << typeToString(it->second.type) << ")" << " = " << valueToString(it->second, true, 0) << std::endl;
    }
}

void GobLang::Machine::printVariablesInfo()
{
    std::cout << "Local(" << m_variables.size() << "):" << std::endl;
    for (std::vector<std::vector<MemoryValue>>::iterator layerIt = m_variables.begin(); layerIt != m_variables.end(); layerIt++)
    {
        std::cout << "Frame: " << layerIt - m_variables.begin() << std::endl;
        for (std::vector<MemoryValue>::iterator it = layerIt->begin(); it != layerIt->end(); it++)
        {
            std::cout << it - layerIt->begin() << ": " << typeToString(it->type) << " = " << valueToString(*it, true, 0) << std::endl;
        }
    }
}

void GobLang::Machine::printStack()
{
    std::cout << "Stack(" << m_operationStack.size() << "):" << std::endl;
    for (std::vector<std::vector<MemoryValue>>::iterator layerIt = m_operationStack.begin(); layerIt != m_operationStack.end(); layerIt++)
    {
        std::cout << "Frame: " << layerIt - m_operationStack.begin() << std::endl;
        for (std::vector<MemoryValue>::reverse_iterator it = layerIt->rbegin(); it != layerIt->rend(); it++)
        {
            std::cout << it - layerIt->rbegin() << ": " << typeToString(it->type) << " = " << valueToString(*it, true, 0) << std::endl;
        }
    }
}

GobLang::MemoryValue *GobLang::Machine::getStackTop()

{
    if (m_operationStack.back().empty())
    {
        return nullptr;
    }
    else
    {
        return &m_operationStack.back()[m_operationStack.size() - 1];
    }
}

GobLang::MemoryValue *GobLang::Machine::getStackTopAndPop()
{
    if (m_operationStack.back().empty())
    {
        throw RuntimeException("Unable to get value from the stack because stack is empty");
    }
    MemoryValue *m = new MemoryValue(_getFromTopAndPop());
    return m;
}

GobLang::ArrayNode *GobLang::Machine::createArrayOfSize(int32_t size)
{
    ArrayNode *node = new ArrayNode(size);
    m_memoryRoot.pushBack(node);
    return node;
}

GobLang::StringNode *GobLang::Machine::createString(std::string const &str, bool alwaysNew)
{
    MemoryNode *root = &m_memoryRoot;
    StringNode *node = nullptr;
    // avoid making instance for each call, check if there is anything that uses this already
    while (root != nullptr && !alwaysNew)
    {
        if (StringNode *strNode = dynamic_cast<StringNode *>(root); strNode != nullptr && strNode->getString() == str)
        {
            node = strNode;
            break;
        }
        root = root->getNext();
    }
    if (node == nullptr)
    {
        node = new StringNode(str);
        m_memoryRoot.pushBack(node);
    }
    return node;
}

void GobLang::Machine::addObject(MemoryNode *obj)
{
    if (!obj->isRegistered())
    {
        obj->registerGC();
        m_memoryRoot.pushBack(obj);
    }
}

void GobLang::Machine::popStack()
{
    m_operationStack.back().pop_back();
}

void GobLang::Machine::pushToStack(MemoryValue const &val)
{
    m_operationStack.back().push_back(val);
}

void GobLang::Machine::pushIntToStack(int32_t val)
{
    m_operationStack.back().push_back(MemoryValue{.type = Type::Int, .value = val});
}

void GobLang::Machine::pushFloatToStack(float val)
{
    m_operationStack.back().push_back(MemoryValue{.type = Type::Float, .value = val});
}

void GobLang::Machine::pushObjectToStack(MemoryNode *obj)
{
    m_operationStack.back().push_back(MemoryValue{.type = Type::MemoryObj, .value = obj});
}

void GobLang::Machine::setLocalVariableValue(size_t id, MemoryValue const &val)
{
    std::vector<MemoryValue> &varFrame = m_variables.back();
    if (id >= varFrame.size())
    {
        varFrame.resize(id + 1);
    }
    if (val.type == Type::MemoryObj)
    {
        std::get<MemoryNode *>(val.value)->increaseRefCount();
    }
    if (varFrame[id].type == Type::MemoryObj)
    {
        std::get<MemoryNode *>(varFrame[id].value)->decreaseRefCount();
    }
    varFrame[id] = val;
}

GobLang::MemoryValue *GobLang::Machine::getLocalVariableValue(size_t id)
{
    if (m_variables.back().size() < id)
    {
        return nullptr;
    }
    return &m_variables.back()[id];
}

void GobLang::Machine::shrinkLocalVariableStackBy(size_t size)
{
    size_t i = 0;
    for (std::vector<MemoryValue>::reverse_iterator it = m_variables.back().rbegin(); it != m_variables.back().rend() && i < size; it++, i++)
    {
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->decreaseRefCount();
        }
    }
    m_variables.back().resize(m_variables.back().size() - size);
}

void GobLang::Machine::removeFunctionFrame()
{
    if (m_variables.size() == 1)
    {
        throw RuntimeException("Attempted to remove root variable stack frame");
    }
    std::vector<MemoryValue> &frame = m_variables.back();
    for (std::vector<MemoryValue>::const_iterator it = frame.begin(); it != frame.end(); it++)
    {
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->decreaseRefCount();
        }
    }
    m_variables.pop_back();
    std::vector<MemoryValue> &stackFrame = m_operationStack.back();
    for (std::vector<MemoryValue>::const_iterator it = stackFrame.begin(); it != stackFrame.end(); it++)
    {
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->decreaseRefCount();
        }
    }
    m_operationStack.pop_back();
}

void GobLang::Machine::callLocalFunction(size_t funcId)
{
    if (funcId > m_functions.size())
    {
        throw RuntimeException(std::string("Attempted to call function with id ") + std::to_string(funcId) + " but no function uses that id");
    }
    m_callStack.push_back(m_programCounter);
    m_programCounter = m_functions[funcId].start - 1;
    std::vector<MemoryValue> args = std::vector<MemoryValue>(m_functions[funcId].arguments.size());

    std::vector<MemoryValue> &variableFrame = m_variables.back();

    for (std::vector<MemoryValue>::reverse_iterator it = args.rbegin(); it != args.rend(); it++)
    {
        *it = _getFromTopAndPop();
        // for the entirety of the value being in the function we assume that it is in use so we can not delete it
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->increaseRefCount();
        }
    }
    m_variables.push_back(args);
    m_operationStack.push_back({});
}

void GobLang::Machine::createVariable(std::string const &name, MemoryValue const &value)
{
    m_globals[name] = value;
}

void GobLang::Machine::createType(std::string const &name, FunctionValue const &constructor, std::map<std::string, FunctionValue> const &methods)
{
    addFunction(constructor, name);
    m_nativeStructures[name] = std::unique_ptr<Struct::NativeStructureInfo>(new NativeStructureInfo{
        .name = name,
        .constructor = constructor,
        .methods = methods});
}

NativeStructureInfo const *GobLang::Machine::getNativeStructure(std::string const &name)
{
    if (m_nativeStructures.count(name))
    {
        return m_nativeStructures[name].get();
    }
    return nullptr;
}

void GobLang::Machine::collectGarbage()
{
    MemoryNode *prev = &m_memoryRoot;
    MemoryNode *curr = m_memoryRoot.getNext();
    while (curr != nullptr)
    {
        if (!curr->isDead())
        {
            prev = curr;
            curr = curr->getNext();
            continue;
        }
        // if we are deleting then prev should stay the same while
        // curr gets deleted
        MemoryNode *del = curr;
        prev->eraseNext();
        curr = prev->getNext();

        delete del;
    }
}

GobLang::Machine::~Machine()
{
    MemoryNode *root = m_memoryRoot.getNext();
    while (root != nullptr)
    {
        MemoryNode *del = root;
        root = root->getNext();
        delete del;
    }
}

GobLang::ProgramAddressType GobLang::Machine::_getAddressFromByteCode(size_t start)
{
    ProgramAddressType reconAddr = 0x0;
    for (int32_t i = 0; i < sizeof(ProgramAddressType); i++)
    {
        ProgramAddressType offset = (sizeof(ProgramAddressType) - i - 1) * 8;
        reconAddr |= (ProgramAddressType)(m_operations[start + i]) << offset;
    }
    return reconAddr;
}

void GobLang::Machine::_jump()
{
    ProgramAddressType dest = _getAddressFromByteCode(m_programCounter + 1);
    m_programCounter = dest + m_programCounter;
}

void GobLang::Machine::_jumpIf()
{
    ProgramAddressType dest = _getAddressFromByteCode(m_programCounter + 1);

    MemoryValue a = _getFromTopAndPop();
    if (a.type == Type::Bool)
    {
        if (!std::get<bool>(a.value))
        {
            m_programCounter = dest + m_programCounter;
        }
        else
        {
            m_programCounter += sizeof(ProgramAddressType) + 1;
        }
    }
    else
    {
        throw RuntimeException(std::string("Invalid data type passed to condition check. Expected bool got: ") + typeToString(a.type));
    }
}

inline void GobLang::Machine::_jumpBack()
{
    ProgramAddressType dest = _getAddressFromByteCode(m_programCounter + 1);
    if (dest > m_programCounter)
    {
        throw RuntimeException("Jump back offset is larger than the PC");
    }
    m_programCounter -= dest;
}

void GobLang::Machine::_add()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to add values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    Value c;
    switch (a.type)
    {
    case Type::Int:
        c = std::get<int32_t>(a.value) + std::get<int32_t>(b.value);
        break;
    case Type::UnsignedInt:
        c = std::get<uint32_t>(a.value) + std::get<uint32_t>(b.value);
        break;
    case Type::Float:
        c = std::get<float>(a.value) + std::get<float>(b.value);
        break;
    case Type::MemoryObj:
    {
        StringNode *str1 = dynamic_cast<StringNode *>(std::get<MemoryNode *>(a.value));
        StringNode *str2 = dynamic_cast<StringNode *>(std::get<MemoryNode *>(b.value));
        if (str1 != nullptr && str2 != nullptr)
        {
            c = createString(str1->getString() + str2->getString());
        }
    }
    break;
    default:
        throw RuntimeException(std::string("Invalid type used for math operation: ") + typeToString(a.type));
    }
    pushToStack(MemoryValue{.type = a.type, .value = c});
}

void GobLang::Machine::_sub()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to add values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    Value c;
    switch (a.type)
    {
    case Type::Int:
        c = std::get<int32_t>(b.value) - std::get<int32_t>(a.value);
        break;
    case Type::UnsignedInt:
        c = std::get<uint32_t>(b.value) - std::get<uint32_t>(a.value);
        break;
    case Type::Float:
        c = std::get<float>(b.value) - std::get<float>(a.value);
        break;
    default:
        throw RuntimeException(std::string("Invalid type used for math operation") + typeToString(a.type));
    }
    pushToStack(MemoryValue{.type = a.type, .value = c});
}

void GobLang::Machine::_mul()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to add values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    Value c;
    switch (a.type)
    {
    case Type::Int:
        c = std::get<int32_t>(b.value) * std::get<int32_t>(a.value);
        break;
    case Type::UnsignedInt:
        c = std::get<uint32_t>(b.value) * std::get<uint32_t>(a.value);
        break;
    case Type::Float:
        c = std::get<float>(b.value) * std::get<float>(a.value);
        break;
    default:
        throw RuntimeException(std::string("Invalid type used for math operation") + typeToString(a.type));
    }
    pushToStack(MemoryValue{.type = a.type, .value = c});
}

void GobLang::Machine::_div()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to add values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    Value c;
    switch (a.type)
    {
    case Type::Int:
        c = std::get<int32_t>(b.value) / std::get<int32_t>(a.value);
        break;
    case Type::UnsignedInt:
        c = std::get<uint32_t>(b.value) / std::get<uint32_t>(a.value);
        break;
    case Type::Float:
        c = std::get<float>(b.value) / std::get<float>(a.value);
        break;
    default:
        throw RuntimeException(std::string("Invalid type used for math operation") + typeToString(a.type));
    }
    pushToStack(MemoryValue{.type = a.type, .value = c});
}

inline void GobLang::Machine::_mod()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException("Type mismatch in modulo operation");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) % std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) % std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
    if (a.type != b.type && a.type != Type::Int)
    {
        throw RuntimeException("Modulo can only be used on Ints");
    }
}

void GobLang::Machine::_set()
{
    // (name val =)
    MemoryValue name = _getFromTopAndPop();
    MemoryValue val = _getFromTopAndPop();
    StringNode *memStr = dynamic_cast<StringNode *>(std::get<MemoryNode *>(name.value));
    if (memStr != nullptr)
    {
        if (val.type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(val.value)->increaseRefCount();
        }
        if (m_globals.count(memStr->getString()) > 0 && m_globals[memStr->getString()].type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(m_globals[memStr->getString()].value)->decreaseRefCount();
        }
        m_globals[memStr->getString()] = val;
    }
}

void GobLang::Machine::_get()
{
    MemoryValue name = _getFromTopAndPop();
    assert(std::holds_alternative<MemoryNode *>(name.value));
    StringNode *memStr = dynamic_cast<StringNode *>(std::get<MemoryNode *>(name.value));
    if (memStr != nullptr)
    {
        if (m_globals.count(memStr->getString()) < 1)
        {
            throw RuntimeException(std::string("Attempted to get variable '" + memStr->getString() + "', which doesn't exist"));
        }
        pushToStack(m_globals[memStr->getString()]);
    }
}

inline void GobLang::Machine::_bitAnd()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type || (a.type != Type::Int && a.type != Type::UnsignedInt))
    {
        throw RuntimeException(std::string("Attempted to bit AND values of ") + typeToString(a.type) + " and " + typeToString(b.type) + ". Only int or unsigned int is allowed");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) & std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) & std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
}

inline void GobLang::Machine::_bitOr()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type || (a.type != Type::Int && a.type != Type::UnsignedInt))
    {
        throw RuntimeException(std::string("Attempted to bit OR values of ") + typeToString(a.type) + " and " + typeToString(b.type) + ". Only int or unsigned int is allowed");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) | std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) | std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
}

inline void GobLang::Machine::_bitXor()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type || (a.type != Type::Int && a.type != Type::UnsignedInt))
    {
        throw RuntimeException(std::string("Attempted to bit XOR values of ") + typeToString(a.type) + " and " + typeToString(b.type) + ". Only int or unsigned int is allowed");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) ^ std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) ^ std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
}

inline void GobLang::Machine::_bitNot()
{
    MemoryValue a = _getFromTopAndPop();
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = ~std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = ~std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException(std::string("Attempted to bit NOT value of  ") + typeToString(a.type) + ". Only int or unsigned int is allowed");
    }
}

inline void GobLang::Machine::_shiftLeft()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type || (a.type != Type::Int && a.type != Type::UnsignedInt))
    {
        throw RuntimeException(std::string("Attempted to bit  bit shift leftvalues of ") + typeToString(a.type) + " and " + typeToString(b.type) + ". Only int or unsigned int is allowed");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) << std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) << std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
}

inline void GobLang::Machine::_shiftRight()
{
    MemoryValue a = _getFromTopAndPop();
    MemoryValue b = _getFromTopAndPop();
    if (a.type != b.type || (a.type != Type::Int && a.type != Type::UnsignedInt))
    {
        throw RuntimeException(std::string("Attempted to bit  bit right leftvalues of ") + typeToString(a.type) + " and " + typeToString(b.type) + ". Only int or unsigned int is allowed");
    }
    switch (a.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = std::get<int32_t>(b.value) >> std::get<int32_t>(a.value)});
        break;
    case Type::UnsignedInt:
        pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = std::get<uint32_t>(b.value) >> std::get<uint32_t>(a.value)});
        break;
    default:
        throw RuntimeException("Modulo can only be used on int or unsigned int");
    }
}

void GobLang::Machine::_setLocal()
{
    MemoryValue val = _getFromTopAndPop();
    m_programCounter++;
    uint8_t id = m_operations[m_programCounter];
    setLocalVariableValue(id, val);
}

void GobLang::Machine::_getLocal()
{
    m_programCounter++;
    uint8_t id = m_operations[m_programCounter];
    if (MemoryValue *val = getLocalVariableValue(id); val != nullptr)
    {
        pushToStack(*val);
    }
    else
    {
        throw RuntimeException(std::string("Attempted to retrieve value of variable ") + std::to_string(id) + ", but no variable uses this id");
    }
}

void GobLang::Machine::_call()
{
    MemoryValue func = _getFromTopAndPop();
    switch (func.type)
    {
    case Type::NativeFunction:
        std::get<FunctionValue>(func.value)(this);
        break;
    case Type::MemoryObj:
        if (FunctionRef *f = dynamic_cast<FunctionRef *>(std::get<MemoryNode *>(func.value)); f != nullptr)
        {
            if (f->isLocal())
            {
                callLocalFunction(f->getLocalFuncId());
                return;
            }
            else
            {
                if (f->hasOwner())
                {
                    pushToStack(MemoryValue{.type = Type::MemoryObj, .value = f->getOwner()});
                    (*f->getFunction())(this);
                }
                else
                {
                    (*f->getFunction())(this);
                }
            }
        }
        else
        {
            throw RuntimeException(std::string("Attempted to call a non-function object of type: ") + typeToString(func.type));
        }
        break;
    default:
        throw RuntimeException(std::string("Attempted to call a non-function object of type: ") + typeToString(func.type));
    }
}

void GobLang::Machine::_getLocalFunc()
{
    m_programCounter++;
    size_t funcId = (size_t)m_operations[m_programCounter];
    FunctionRef *f = new FunctionRef(funcId);
    addObject(f);
    pushObjectToStack(f);
}

void GobLang::Machine::_return()
{
    size_t pos = m_callStack.back();
    m_callStack.pop_back();
    m_programCounter = pos;
    removeFunctionFrame();
}

void GobLang::Machine::_returnWithValue()
{
    size_t pos = m_callStack.back();
    m_callStack.pop_back();
    m_programCounter = pos;
    // we have to remove it manually to avoid it getting grabbed by the garbage collector
    MemoryValue returnVal = _getFromTopAndPop();
    removeFunctionFrame();
    pushToStack(returnVal);
}

void GobLang::Machine::_pushConstInt()
{
    int32_t val = _parseOperationConstant<int32_t>(m_programCounter + 1);
    m_programCounter += sizeof(int32_t);
    pushToStack(MemoryValue{.type = Type::Int, .value = val});
}

inline void GobLang::Machine::_pushConstUnsignedInt()
{
    uint32_t val = _parseOperationConstant<uint32_t>(m_programCounter + 1);
    m_programCounter += sizeof(uint32_t);
    pushToStack(MemoryValue{.type = Type::UnsignedInt, .value = val});
}

void GobLang::Machine::_pushConstFloat()
{
    float val = _parseOperationConstant<float>(m_programCounter + 1);
    m_programCounter += sizeof(float);
    pushToStack(MemoryValue{.type = Type::Float, .value = val});
}

void GobLang::Machine::_pushConstChar()
{
    pushToStack(MemoryValue{.type = Type::Char, .value = (char)m_operations[m_programCounter + 1]});
    m_programCounter++;
}

void GobLang::Machine::_pushConstString()
{
    std::string &str = m_constStrings[(size_t)m_operations[m_programCounter + 1]];
    // we always create a new string object because otherwise each variable will share same pointer to constant string which can be altered
    StringNode *node = createString(str, true);

    m_programCounter++;
    pushToStack(MemoryValue{.type = Type::MemoryObj, .value = node});
}

void GobLang::Machine::_pushConstNull()
{
    pushToStack(MemoryValue{.type = Type::Null, .value = 0});
}

void GobLang::Machine::_getArray()
{
    MemoryValue array = _getFromTopAndPop();
    MemoryValue index = _getFromTopAndPop();
    if (!std::holds_alternative<MemoryNode *>(array.value))
    {
        throw RuntimeException(std::string("Attempted to get array value, but array has instead type: ") + typeToString(array.type));
    }
    if (!std::holds_alternative<int32_t>(index.value))
    {
        throw RuntimeException(std::string("Attempted to get array value, but index has instead type: ") + typeToString(array.type));
    }
    if (ArrayNode *arrNode = dynamic_cast<ArrayNode *>(std::get<MemoryNode *>(array.value)); arrNode != nullptr)
    {
        pushToStack(*arrNode->getItem(std::get<int32_t>(index.value)));
    }
    else if (StringNode *strNode = dynamic_cast<StringNode *>(std::get<MemoryNode *>(array.value)); strNode != nullptr)
    {
        pushToStack(MemoryValue{.type = Type::Char, .value = strNode->getCharAt(std::get<int32_t>(index.value))});
    }
}

void GobLang::Machine::_setArray()
{
    MemoryValue value = _getFromTopAndPop();
    MemoryValue array = _getFromTopAndPop();
    MemoryValue index = _getFromTopAndPop();
    if (!std::holds_alternative<MemoryNode *>(array.value))
    {
        throw RuntimeException(std::string("Attempted to set array value, but array has instead type: ") + typeToString(array.type));
    }
    if (!std::holds_alternative<int32_t>(index.value))
    {
        throw RuntimeException(std::string("Attempted to set array value, but index has instead type: ") + typeToString(array.type));
    }
    MemoryNode *m = std::get<MemoryNode *>(array.value);
    if (ArrayNode *arrNode = dynamic_cast<ArrayNode *>(m); arrNode != nullptr)
    {
        arrNode->setItem(std::get<int32_t>(index.value), value);
    }
    else if (StringNode *strNode = dynamic_cast<StringNode *>(m); strNode != nullptr && value.type == Type::Char)
    {
        strNode->setCharAt(std::get<char>(value.value), std::get<int32_t>(index.value));
    }
}

inline void GobLang::Machine::_getField()
{
    MemoryValue object = _getFromTopAndPop();
    MemoryValue field = _getFromTopAndPop();
    if (!std::holds_alternative<MemoryNode *>(object.value))
    {
        throw RuntimeException(std::string("Attempted to getfield, but object has instead type: ") + typeToString(object.type));
    }
    if (!std::holds_alternative<MemoryNode *>(field.value))
    {
        throw RuntimeException(std::string("Attempted to get field, but field name has instead type: ") + typeToString(field.type));
    }
    if (StructureObjectNode *arrNode = dynamic_cast<StructureObjectNode *>(std::get<MemoryNode *>(object.value)); arrNode != nullptr)
    {
        if (StringNode *strNode = dynamic_cast<StringNode *>(std::get<MemoryNode *>(field.value)); strNode != nullptr)
        {
            MemoryValue v = arrNode->getField(strNode->getString());
            if (std::holds_alternative<MemoryNode *>(v.value))
            {
                addObject(std::get<MemoryNode *>(v.value));
            }
            pushToStack(v);
        }
        else
        {
            throw RuntimeException("Attempted to get field on a but field name is not a string");
        }
    }
    else
    {
        throw RuntimeException("Attempted to get field on a non-struct object");
    }
}

inline void GobLang::Machine::_setField()
{
    MemoryValue value = _getFromTopAndPop();
    MemoryValue object = _getFromTopAndPop();
    MemoryValue field = _getFromTopAndPop();
    if (!std::holds_alternative<MemoryNode *>(object.value))
    {
        throw RuntimeException(std::string("Attempted to getfield, but object has instead type: ") + typeToString(object.type));
    }
    if (!std::holds_alternative<MemoryNode *>(field.value))
    {
        throw RuntimeException(std::string("Attempted to get field, but field name has instead type: ") + typeToString(field.type));
    }
    if (StructureObjectNode *arrNode = dynamic_cast<StructureObjectNode *>(std::get<MemoryNode *>(object.value)); arrNode != nullptr)
    {
        if (StringNode *strNode = dynamic_cast<StringNode *>(std::get<MemoryNode *>(field.value)); strNode != nullptr)
        {
            arrNode->setField(strNode->getString(), value);
        }
    }
}

inline void GobLang::Machine::_callMethod()
{
    MemoryValue methodName = _getFromTopAndPop();
    MemoryValue object = _getFromTopAndPop();

    if (!std::holds_alternative<MemoryNode *>(object.value))
    {
        throw RuntimeException(std::string("Attempted to getfield, but object has instead type: ") + typeToString(object.type));
    }
    if (!std::holds_alternative<MemoryNode *>(methodName.value))
    {
        throw RuntimeException(std::string("Attempted to get field, but field name has instead type: ") + typeToString(methodName.type));
    }

    std::string name;

    if (StringNode *strNode = dynamic_cast<StringNode *>(std::get<MemoryNode *>(methodName.value)); strNode != nullptr)
    {
        name = strNode->getString();
    }

    if (StructureObjectNode *objNode = dynamic_cast<StructureObjectNode *>(std::get<MemoryNode *>(object.value)); objNode != nullptr)
    {
        // this is to simulate "this" argument
        // or if comparing to python, passing "self" argument
        pushToStack(object);
        if (objNode->hasNativeMethod(name))
        {
            (*objNode->getNativeMethod(name))(this);
        }
        else
        {
            throw RuntimeException(std::string("Attempted to call a goblang method '") + name + "' but only natively binded methods are supported");
        }
    }
}

void GobLang::Machine::_eq()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type == b.type || a.type == Type::Null || b.type == Type::Null)
    {
        pushToStack(MemoryValue{.type = Type::Bool, .value = areEqual(a, b)});
    }
    else
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
}

void GobLang::Machine::_neq()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type && !(a.type == Type::Null || b.type == Type::Null))
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        pushToStack(MemoryValue{.type = Type::Bool, .value = !areEqual(a, b)});
    }
}

void GobLang::Machine::_and()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type && a.type != Type::Bool)
    {
        throw RuntimeException(std::string("Attempted to 'and' values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<bool>(a.value) && std::get<bool>(b.value)});
    }
}

void GobLang::Machine::_or()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type && a.type != Type::Bool)
    {
        throw RuntimeException(std::string("Attempted to 'or' values of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<bool>(a.value) || std::get<bool>(b.value)});
    }
}

void GobLang::Machine::_less()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        switch (a.type)
        {
        case Type::Int:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<int32_t>(a.value) < std::get<int32_t>(b.value)});
            break;
        case Type::Float:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<float>(a.value) < std::get<float>(b.value)});
            break;
        default:
            throw RuntimeException(std::string("Attempted to compare value of type ") + typeToString(a.type) + ". Only numeric types can be compared using >,<, <=, >=");
        }
    }
}

void GobLang::Machine::_more()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        switch (a.type)
        {
        case Type::Int:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<int32_t>(a.value) > std::get<int32_t>(b.value)});
            break;
        case Type::Float:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<float>(a.value) > std::get<float>(b.value)});
            break;
        default:
            throw RuntimeException(std::string("Attempted to compare value of type ") + typeToString(a.type) + ". Only numeric types can be compared using >,<, <=, >=");
        }
    }
}

void GobLang::Machine::_lessOrEq()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        switch (a.type)
        {
        case Type::Int:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<int32_t>(a.value) <= std::get<int32_t>(b.value)});
            break;
        case Type::Float:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<float>(a.value) <= std::get<float>(b.value)});
            break;
        default:
            throw RuntimeException(std::string("Attempted to compare value of type ") + typeToString(a.type) + ". Only numeric types can be compared using >,<, <=, >=");
        }
    }
}

void GobLang::Machine::_moreOrEq()
{
    MemoryValue b = _getFromTopAndPop();
    MemoryValue a = _getFromTopAndPop();
    if (a.type != b.type)
    {
        throw RuntimeException(std::string("Attempted to compare value of ") + typeToString(a.type) + " and " + typeToString(b.type));
    }
    else
    {
        switch (a.type)
        {
        case Type::Int:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<int32_t>(a.value) >= std::get<int32_t>(b.value)});
            break;
        case Type::Float:
            pushToStack(MemoryValue{.type = Type::Bool, .value = std::get<float>(a.value) >= std::get<float>(b.value)});
            break;
        default:
            throw RuntimeException(std::string("Attempted to compare value of type ") + typeToString(a.type) + ". Only numeric types can be compared using >,<, <=, >=");
        }
    }
}

void GobLang::Machine::_negate()
{
    MemoryValue val = _getFromTopAndPop();
    switch (val.type)
    {
    case Type::Int:
        pushToStack(MemoryValue{.type = Type::Int, .value = -std::get<int32_t>(val.value)});
        break;
    case Type::Float:
        pushToStack(MemoryValue{.type = Type::Float, .value = -std::get<float>(val.value)});
        break;
    default:
        throw RuntimeException("Attempted to apply negate operation on a non numeric value");
    }
}

void GobLang::Machine::_not()
{
    MemoryValue val = _getFromTopAndPop();
    if (val.type != Type::Bool)
    {
        throw RuntimeException("Attempted to negate non boolean value");
    }
    pushToStack(MemoryValue{.type = Type::Bool, .value = !std::get<bool>(val.value)});
}

void GobLang::Machine::_shrink()
{
    m_programCounter++;
    size_t amount = (size_t)m_operations[m_programCounter];
    shrinkLocalVariableStackBy(amount);
}

void GobLang::Machine::_createArray()
{
    m_programCounter++;
    int32_t arraySize = m_operations[m_programCounter];
    ArrayNode *array = createArrayOfSize(arraySize);
    for (int32_t i = arraySize - 1; i >= 0; i--)
    {
        array->setItem(i, _getFromTopAndPop());
    }
    pushToStack(MemoryValue{.type = Type::MemoryObj, .value = array});
}

inline void GobLang::Machine::_new()
{
    m_programCounter++;
    size_t structId = m_operations[m_programCounter];

    StructureObjectNode *obj = new StructureObjectNode(m_structures[structId].get());
    addObject(obj);
    pushToStack(MemoryValue{.type = Type::MemoryObj, .value = obj});
}
