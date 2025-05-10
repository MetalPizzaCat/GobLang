#pragma once
#include <string>
#include <map>
#include "Type.hpp"
#include "Value.hpp"
#include "Structure.hpp"
#include "Memory.hpp"
namespace GobLang::Struct
{
    struct NativeStructureInfo
    {
        std::string name;
        FunctionValue constructor;
        std::map<std::string, FunctionValue> methods;
    };

    class NativeStructureObjectNode : public MemoryNode
    {
    public:
        explicit NativeStructureObjectNode(
            NativeStructureInfo const *info,
            Structure const *baseInfo = nullptr) : MemoryNode(baseInfo), m_nativeStruct(info) {}

        Value getField(std::string const &field) override;


    private:
        NativeStructureInfo const *m_nativeStruct;
    };
} // namespace GobLang
