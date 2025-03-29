#pragma once
#include <vector>
#include <cstdint>
namespace GobLang::Compiler
{
    template <typename T>
    T parseBytesIntoValue(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
    {
        uint64_t res = 0;
        for (uint64_t i = 0; i < sizeof(T) && (begin + i != end); i++)
        {
            uint64_t offset = (sizeof(T) - i - 1) * 8;
            res |= (uint64_t)(*(begin + i)) << offset;
        }
        T *f = reinterpret_cast<T *>(&res);
        return *f;
    }
    void byteCodeToText(std::vector<uint8_t> const &bytecode)
    {
        size_t address = 0;
        for (std::vector<uint8_t>::const_iterator it = bytecode.begin(); it != bytecode.end(); it++)
        {
            std::vector<OperationData>::const_iterator opIt = std::find_if(
                Operations.begin(),
                Operations.end(),
                [it](OperationData const &a)
                {
                    return (uint8_t)a.op == *it;
                });
            if (opIt != Operations.end())
            {
                std::cout << std::hex << address << std::dec << ": " << (opIt->text) << " ";
                switch (opIt->argType)
                {
                case OperatorArgType::Char:
                    it++;
                    address++;
                    std::cout << '\'' << (char)(*it) << '\'';
                    break;
                case OperatorArgType::Byte:
                    it++;
                    address++;
                    std::cout << std::to_string(*it);
                    break;
                case OperatorArgType::Float:
                {
                    float val = parseBytesIntoValue<float>(it + 1, bytecode.end());
                    it += sizeof(float);
                    address += sizeof(float);
                    std::cout << val;
                }
                break;
                case OperatorArgType::Int:
                {
                    int32_t val = parseBytesIntoValue<int32_t>(it + 1, bytecode.end());
                    it += sizeof(int32_t);
                    address += sizeof(int32_t);
                    std::cout << val;
                }
                break;
                case OperatorArgType::Address:
                {
                    ProgramAddressType val = parseBytesIntoValue<ProgramAddressType>(it + 1, bytecode.end());
                    it += sizeof(ProgramAddressType);
                    address += sizeof(ProgramAddressType);
                    std::cout << std::hex << val << std::dec;
                }
                break;
                case OperatorArgType::UnsignedInt:
                {
                    uint32_t val = parseBytesIntoValue<uint32_t>(it + 1, bytecode.end());
                    it += sizeof(uint32_t);
                    address += sizeof(uint32_t);
                    std::cout << val;
                }
                break;
                default:
                    break;
                }
                address++;
                std::cout << std::endl;
            }
        }
    }
}