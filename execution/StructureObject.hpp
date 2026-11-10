#pragma once
#include "Memory.hpp"
#include "Structure.hpp"

namespace GobLang
{
    class StructureObject : public Object
    {
    public:
        std::string toString() const override;
        virtual ~StructureObject() = default;

    private:
        Struct::Structure const *m_class;
    };
} // namespace GobLang
