#pragma once
#include "Parser.hpp"
#include <vector>
#include <cstdint>
#include "ByteCode.hpp"
#include "CompilerToken.hpp"
#include "CompilerNode.hpp"
namespace SimpleLang::Compiler
{
    class Compiler
    {
    public:
        explicit Compiler(Parser const &parser) : m_parser(parser) {}

        /**
         * @brief Convert given parsed data into reverse polish notation representation of code
         *
         */
        void compile();

        /**
         * @brief Generate byte code that can be used by the interpreter and write it into the `m_byteCode` variable
         *
         */
        void generateByteCode();

        /**
         * @brief Dump all contents of the stack into the reverse polish notation array
         *
         */
        void dumpStack();

        void dumpStackWhile(std::function<bool(Token *)> const &cond);

        /**
         * @brief Get priority for the top item on the stack or -1 if stack is empty
         *
         * @return int32_t Priority
         */
        int32_t getTopStackPriority();

        /**
         * @brief Pop the last item from the stack and return it
         *
         * @return Token* Token from top of the stack or nullptr if stack is empty
         */
        Token *popStack();

        /**
         * @brief Prints the parsed sequence using the token data
         *
         */
        void printCode();

        static std::vector<uint8_t> generateGetByteCode(Token *token);

        static std::vector<uint8_t> generateSetByteCode(Token *token);

        //void appendByteCode(std::vector<uint8_t> const &code);

        /**
         * @brief Generate and add byte code for the given compiler node. If node has mark attached this mark will be updated
         * 
         * @param node Node to process
         * @param getter If true getter code will be generated, otherwise false
         */
        void appendCompilerNode(CompilerNode *node, bool getter);

        void addNewMarkReplacement(size_t mark, size_t address);

        ByteCode getByteCode() const { return m_byteCode; }

        size_t getMarkCounterAndAdvance();

        ~Compiler();

    private:
        void _placeAddressForMark(size_t mark, size_t address, bool erase);
        void _compileSeparators(SeparatorToken *sepToken, std::vector<Token *>::const_iterator const &it);

        void _compileKeywords(KeywordToken *keyToken, std::vector<Token *>::const_iterator const &it);

        bool _isElseChainToken(std::vector<Token *>::const_iterator const &it);

        bool _isElifChainToken(std::vector<Token *>::const_iterator const &it);
        /**
         * @brief code representation in reverse polish notation
         *
         */
        std::vector<Token *> m_code;
        std::vector<uint8_t> m_bytes;

        std::vector<Token *> m_stack;

        std::vector<GotoToken *> m_jumps;

        /**
         * @brief Jump map used in bytecode generation to know which places require which marks.
         * key is mark id, value are all places which require address replacement
         *
         */
        std::map<size_t, std::vector<size_t>> m_jumpMarks;
        
        std::map<size_t, size_t> m_jumpDestinations;
        /**
         * @brief Array for storing all tokens created during the conversion process, used only to be able to delete them once the process ends
         *
         */
        std::vector<Token *> m_compilerTokens;

        std::vector<FunctionCallToken *> m_functionCalls;

        ByteCode m_byteCode;
        Parser const &m_parser;

        bool m_isInConditionHead = false;
        size_t m_markCounter = 0;
    };

}