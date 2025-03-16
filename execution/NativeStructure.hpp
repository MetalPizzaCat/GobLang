#pragma once
#include <string>
#include <map>
#include "Type.hpp"
#include "Value.hpp"
#include "Structure.hpp"
namespace GobLang::Struct
{
    struct NativeStructureInfo
    {
        std::string name;
        FunctionValue constructor;
        std::map<std::string, FunctionValue> methods;
    };

    class NativeStructureObjectNode : public StructureObjectNode
    {
    public:
        explicit NativeStructureObjectNode(
            NativeStructureInfo const *info,
            Structure const *baseInfo = nullptr) : StructureObjectNode(baseInfo), m_nativeStruct(info) {}

        bool hasNativeMethod(std::string const &name) override;

        FunctionValue const *getNativeMethod(std::string const &name) const override;

    private:
        NativeStructureInfo const *m_nativeStruct;
    };
} // namespace GobLang
