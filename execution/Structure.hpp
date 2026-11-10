#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <map>
#include "Value.hpp"
#include "Type.hpp"

namespace GobLang::Struct
{

    // enum class StructureFieldType
    // {
    //     Char,
    //     Bool,
    //     Float,
    //     Int,
    //     UnsignedInt,
    //     MemoryObj,
    //     Array,
    //     String
    // };

    // struct Field
    // {
    //     std::string name;
    //     StructureFieldType type;
    //     /// @brief Reference to the custom type if type is MemoryObj
    //     size_t customStructureTypeId;
    // };

    /**
     * @brief Class that describes
     *
     */
    class Structure
    {
    public:
        std::optional<Value> getField(std::string const &name) const;
        std::string const &getName() const { return m_name; }

    private:
        std::string m_name;
        /// @brief Base fields present in every instance of this stucture. Although this could be used for anything, this is primarily meant for methods
        std::unordered_map<std::string, Value> m_baseFields;
        /// @brief Methods that would be called if instance of this struct is used with `()` operator
        Closure const *m_callable = nullptr;
        /// @brief Method that is called on each instantiation
        Closure const *m_constructor = nullptr;
        /// @brief Structure that this one inherits from
        Structure const *m_parent = nullptr;
    };

}