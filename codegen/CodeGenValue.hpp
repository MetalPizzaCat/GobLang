#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <bit>
#include "../execution/Function.hpp"

namespace GobLang::Codegen
{
    template <typename T>
    std::vector<uint8_t> parseToBytes(T val)
    {
        uint32_t const v =  std::bit_cast<uint32_t>(val);
        std::vector<uint8_t> res;
        for (int32_t i = sizeof(T) - 1; i >= 0; i--)
        {
            uint64_t offset = (sizeof(uint8_t) * i) * 8;
            const size_t mask = 0xff;
            uint64_t num = v & (mask << offset);
            uint64_t numFixed = num >> offset;
            res.push_back((uint8_t)numFixed);
        }
        return res;
    }

    class BlockContext
    {
    public:
        explicit BlockContext() = default;

        /// @brief Create block context for a given function
        /// @param funcId Id of a function
        /// @param initialVariables Variables that should be added to the block
        explicit BlockContext(size_t funcId, std::vector<size_t> const &initialVariables);

        size_t getVariableCount() const { return m_variables.size(); }

        void insertVariable(size_t name);

        size_t getVariableLocalId(size_t nameId);

        bool hasVariableWithNameId(size_t nameId);

        void insert(std::vector<uint8_t> const &bytes);

        std::vector<uint8_t> const &getBytes() const { return m_bytes; }

        /// @brief Append instructions for clearing variable stack
        void appendMemoryClear();

        bool isFunction() const { return m_funcId != -1; }

        void addJump(bool isBreak);

        void addJumpAt(size_t offset, bool isBreak);

        void createBaseJumpOffset(size_t offset);

        void markAsLoopBlock() { m_loopBlock = true; }

        bool isLoopBlock() const { return m_loopBlock; }

        std::map<size_t, bool> const &getJumps() const { return m_jumps; }

    private:
        std::map<size_t, bool> m_jumps;
        std::vector<size_t> m_variables;
        std::vector<uint8_t> m_bytes;
        size_t m_baseJumpOffset = 0;
        size_t m_funcId = -1;
        bool m_loopBlock;
    };

    class CodeGenValue
    {
    public:
        virtual ~CodeGenValue() = default;

        virtual std::vector<uint8_t> getGetOperationBytes() = 0;

        virtual std::vector<uint8_t> getSetOperationBytes() { return {}; }
    };

    class BlockCodeGenValue : public CodeGenValue
    {
    public:
        explicit BlockCodeGenValue(std::unique_ptr<BlockContext> block);

        std::vector<uint8_t> getGetOperationBytes() override { return m_block->getBytes(); }

        BlockContext const *getBlock() const { return m_block.get(); }

        size_t getBlockSize() const { return m_block->getBytes().size(); }

    private:
        std::unique_ptr<BlockContext> m_block;
    };

    class FunctionCodeGenValue : public CodeGenValue
    {
    public:
        explicit FunctionCodeGenValue(Function const *func, std::unique_ptr<BlockContext> body);
        std::vector<uint8_t> getGetOperationBytes() override;
        Function const *getFuncInfo() const { return m_func; }

    private:
        std::unique_ptr<BlockContext> m_body;
        Function const *m_func;
    };

    class FunctionPrototypeCodeGenValue : public CodeGenValue
    {
    public:
        explicit FunctionPrototypeCodeGenValue(Function const *func);
        Function const *getFunc() const { return m_func; }
        std::vector<uint8_t> getGetOperationBytes() override { return {}; }

    private:
        Function const *m_func;
    };

    class GeneratedCodeGenValue : public CodeGenValue
    {
    public:
        explicit GeneratedCodeGenValue(std::vector<uint8_t> val);

        std::vector<uint8_t> getGetOperationBytes() override { return m_bytes; }

    private:
        std::vector<uint8_t> m_bytes;
    };

    class BranchCodeGenValue : public CodeGenValue
    {
    public:
        explicit BranchCodeGenValue(std::vector<uint8_t> cond, std::vector<uint8_t> body);

        /// @brief Add jump at the end of the body block(For use with elif and else)
        /// @param offset
        void addJump(size_t offset);

        void addJumpBack(size_t offset);

        std::vector<uint8_t> getGetOperationBytes() override;

        /// @brief Set value for the jump that occurs if condition is evaluated to false
        /// @param offset
        void setConditionJumpOffset(size_t offset);

        /// @brief Get size of the body block in bytes, accounting for the optional end jump
        /// @return
        size_t getBodySize() const;

        /// @brief Get the full size of the block in bytes
        /// @return
        size_t getFullSize() const;

        size_t getConditionSize() const;

        bool hasEndJump() const { return m_jumpAfter != -1; }

    private:
        size_t m_jumpAfter = -1;
        bool m_backwards = false;
        std::vector<uint8_t> m_condBytes;
        std::vector<uint8_t> m_bodyBytes;
    };

    class VariableCodeGenValue : public CodeGenValue
    {
    public:
        explicit VariableCodeGenValue(size_t nameId, bool local);

        std::vector<uint8_t> getGetOperationBytes() override;

        std::vector<uint8_t> getSetOperationBytes() override;

    private:
        size_t m_id;
        bool m_local;
    };

    class ArrayAccessCodeGenValue : public CodeGenValue
    {
    public:
        explicit ArrayAccessCodeGenValue(std::vector<uint8_t> value, std::vector<uint8_t> address);
        std::vector<uint8_t> getGetOperationBytes() override;

        std::vector<uint8_t> getSetOperationBytes() override;

    private:
        std::vector<uint8_t> m_valueBytes;
        std::vector<uint8_t> m_addressBytes;
    };
} // namespace GobLang::Codegen
