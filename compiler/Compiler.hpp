#pragma once
#include "ReversePolishGenerator.hpp"
#include <vector>
#include <cstdint>
#include "ByteCode.hpp"

#include "CompilerNode.hpp"
namespace GobLang::Compiler
{
    template <typename T>
    std::vector<uint8_t> parseToBytes(T val)
    {
        uint32_t *v = reinterpret_cast<uint32_t *>(&val);
        std::vector<uint8_t> res;
        for (int32_t i = sizeof(T) - 1; i >= 0; i--)
        {
            uint64_t offset = (sizeof(uint8_t) * i) * 8;
            const size_t mask = 0xff;
            uint64_t num = (*v) & (mask << offset);
            uint64_t numFixed = num >> offset;
            res.push_back((uint8_t)numFixed);
        }
        return res;
    }

    class Compiler
    {
    public:
        explicit Compiler(ReversePolishGenerator const &generator) : m_generator(generator) {}

        /**
         * @brief Generate byte code that can be used by the interpreter and write it into the `m_byteCode` variable
         *
         */
        void generateByteCode();

        static std::vector<uint8_t> generateGetByteCode(Token *token);

        static std::vector<uint8_t> generateSetByteCode(Token *token);

        // void appendByteCode(std::vector<uint8_t> const &code);

        /**
         * @brief Generate and add byte code for the given compiler node. If node has mark attached this mark will be updated
         *
         * @param node Node to process
         * @param getter If true getter code will be generated, otherwise false
         */
        void appendCompilerNode(CompilerNode *node, bool getter);

        void addNewMarkReplacement(size_t mark, size_t address);

        void addFunctionAddressUsage(size_t funcNameId, size_t address);

        void appendByteCode(std::vector<uint8_t> const &bytes);

        ByteCode getByteCode() const { return m_byteCode; }

        void printLocalFunctionInfo();

    private:
        void _generateBytecodeFor(std::vector<Token *> const &tokens, bool createHaltInstruction);
        void _placeAddressForMark(size_t mark, size_t address, bool erase);

        void _generateGoto(
            GotoToken const *jmpToken,
            std::vector<CompilerNode *> &stack,
            std::vector<Token *>::const_iterator &it);
        void _generateMultiArg(
            MultiArgToken const *multiTok,
            std::vector<CompilerNode *> &stack,
            std::vector<Token *>::const_iterator &it);
        void _generateOperator(
            OperatorToken const *opToken,
            std::vector<CompilerNode *> &stack,
            std::vector<Token *>::const_iterator &it);

        std::vector<uint8_t> m_bytes;

        /**
         * @brief Jump map used in bytecode generation to know which places require which marks.
         * key is mark id, value are all places which require address replacement
         *
         */
        std::map<size_t, std::vector<size_t>> m_jumpMarks;

        /**
         * @brief List of all addresses to replace with future marks. Key is where to write the address and value is what address to write
         *
         */
        std::map<size_t, size_t> m_jumpDestinations;

        /**
         * @brief List of all addresses to replace with future function addresses. Key is where to write the address and value is what address to write
         *
         */
        std::map<size_t, std::vector<size_t>> m_functionCallDestinations;

        ByteCode m_byteCode;

        ReversePolishGenerator const &m_generator;
    };

}