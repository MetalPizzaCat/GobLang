#pragma once
#include <vector>
namespace GobLang::Compiler
{
    struct SpecialCharacter
    {
        const char *sequence;
        char character;
    };

    static const std::vector<SpecialCharacter> SpecialCharacters = {
        SpecialCharacter{.sequence = "\\n", .character = '\n'},
        SpecialCharacter{.sequence = "\\\"", .character = '\"'},
        SpecialCharacter{.sequence = "\\'", .character = '\''},
        SpecialCharacter{.sequence = "\\t", .character = '\t'},
        SpecialCharacter{.sequence = "\\\\", .character = '\\'}};

} // namespace SimpleLang::Compiler
