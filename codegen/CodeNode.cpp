#include "CodeNode.hpp"
#include <algorithm>

#include "Lexems.hpp"
#include "../execution/Value.hpp"
#include "Parser.hpp"
GobLang::Codegen::IdNode::IdNode(size_t id) : m_id(id)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::IdNode::generateCode(Builder &builder)
{
    if (builder.hasLocalFunctionWithName(m_id))
    {
        return builder.createLocalFunctionAccess(m_id);
    }
    return builder.createVariableAccess(m_id);
}

std::string GobLang::Codegen::IdNode::toString()
{
    return "{\"id\": " + std::to_string(m_id) + "}";
}

GobLang::Codegen::FloatNode::FloatNode(float val) : m_val(val)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::FloatNode::generateCode(Builder &builder)
{
    return builder.createConstFloat(m_val);
}

std::string GobLang::Codegen::FloatNode::toString()
{
    return std::to_string(m_val);
}

GobLang::Codegen::IntNode::IntNode(int32_t val) : m_val(val)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::IntNode::generateCode(Builder &builder)
{
    return builder.createConstInt(m_val);
}

std::string GobLang::Codegen::IntNode::toString()
{
    return std::to_string(m_val);
}

GobLang::Codegen::UnsignedIntNode::UnsignedIntNode(uint32_t val) : m_val(val)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::UnsignedIntNode::generateCode(Builder &builder)
{
    return std::unique_ptr<CodeGenValue>();
}

std::string GobLang::Codegen::UnsignedIntNode::toString()
{
    return std::to_string(m_val);
}

GobLang::Codegen::StringNode::StringNode(size_t id) : m_id(id)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::StringNode::generateCode(Builder &builder)
{
    return builder.createConstString(m_id);
}

std::string GobLang::Codegen::StringNode::toString()
{
    return "{\"str\": " + std::to_string(m_id) + "}";
}

GobLang::Codegen::SequenceNode::SequenceNode(std::vector<std::unique_ptr<CodeNode>> seq) : m_sequence(std::move(seq))
{
}

std::unique_ptr<GobLang::Codegen::BlockCodeGenValue> GobLang::Codegen::SequenceNode::generateBlockContext(
    Builder &builder,
    size_t jumpStartOffset,
    bool isLoop)
{
    builder.pushEmptyBlock();
    BlockContext *block = builder.getCurrentBlock();
    if (isLoop)
    {
        block->markAsLoopBlock();
    }
    block->createBaseJumpOffset(jumpStartOffset);
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_sequence.begin(); it != m_sequence.end(); it++)
    {
        block->insert((*it)->generateCode(builder)->getGetOperationBytes());
    }
    block->appendMemoryClear();
    std::unique_ptr<BlockContext> generatedBlock = builder.popBlock();
    if (!generatedBlock->isLoopBlock())
    {
        if (BlockContext *topBlock = builder.getCurrentBlock(); topBlock != nullptr)
        {
            for (auto [addr, isBreak] : generatedBlock->getJumps())
            {
                topBlock->addJumpAt(addr, isBreak);
            }
        }
    }
    return std::make_unique<BlockCodeGenValue>(std::move(generatedBlock));
}

std::string GobLang::Codegen::SequenceNode::toString()
{
    std::string base = "[";
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_sequence.begin(); it != m_sequence.end(); it++)
    {
        base += (*it)->toString();
        if (it + 1 != m_sequence.end())
        {
            base += ',';
        }
    }
    return base + "]";
}

GobLang::Codegen::BinaryOperationNode::BinaryOperationNode(Operator op, std::unique_ptr<CodeNode> left, std::unique_ptr<CodeNode> right) : m_op(op), m_left(std::move(left)), m_right(std::move(right))
{
}

std::string GobLang::Codegen::BinaryOperationNode::toString()
{
    const char *symb = std::find_if(Operators.begin(), Operators.end(), [this](OperatorData const &op)
                                    { return op.op == m_op; })
                           ->symbol;
    return R"({"type" : "op", "left":)" + m_left->toString() + ", \"right\": " + m_right->toString() + ", \"op\":\"" + symb + "\"}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::BinaryOperationNode::generateCode(Builder &builder)
{
    switch (m_op)
    {

    case Operator::AddAssign:
    case Operator::SubAssign:
    case Operator::MulAssign:
    case Operator::DivAssign:
    case Operator::BitAndAssign:
    case Operator::BitNotAssign:
    case Operator::BitXorAssign:
    case Operator::ModuloAssign:
    case Operator::BitRightShiftAssign:
    case Operator::BitLeftShiftAssign:
    {
        std::unique_ptr<CodeGenValue> oper = builder.createOperation(m_left->generateCode(builder),
                                                                     m_right->generateCode(builder),
                                                                     AssignmentAndActionOperatorOrigins.at(m_op));

        return builder.createAssignment(m_left->generateCode(builder), std::move(oper));
    }
    // handle assignment first
    case Operator::Assign:
        return builder.createAssignment(m_left->generateCode(builder), m_right->generateCode(builder));
    default:
        return builder.createOperation(m_left->generateCode(builder), m_right->generateCode(builder), m_op);
    }
}

GobLang::Codegen::FunctionCallNode::FunctionCallNode(std::unique_ptr<CodeNode> value,
                                                     std::vector<std::unique_ptr<CodeNode>> args) : m_value(std::move(value)),
                                                                                                    m_args(std::move(args))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::FunctionCallNode::generateCode(Builder &builder)
{
    std::vector<std::unique_ptr<CodeGenValue>> args;
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_args.begin(); it != m_args.end(); it++)
    {
        args.push_back((*it)->generateCode(builder));
    }
    return builder.createCallFromValue(std::make_unique<GeneratedCodeGenValue>(m_value->generateCode(builder)->getGetOperationBytes()), std::move(args));
}

std::string GobLang::Codegen::FunctionCallNode::toString()
{
    std::string base = R"({ "type" : "call", "name":)" + m_value->toString() + ", \"args\": [";
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_args.begin(); it != m_args.end(); it++)
    {
        base += (*it)->toString();
        if (it + 1 != m_args.end())
        {
            base += ',';
        }
    }
    return base + "]}";
    return std::string();
}

GobLang::Codegen::VariableCreationNode::VariableCreationNode(size_t id, std::unique_ptr<CodeNode> body) : m_id(id), m_body(std::move(body))
{
}

std::string GobLang::Codegen::VariableCreationNode::toString()
{
    return R"({"type" : "let", "var": )" + std::to_string(m_id) + ", \"body\" : " + m_body->toString() + "}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::VariableCreationNode::generateCode(Builder &builder)
{
    size_t var = builder.insertVariable(m_id);
    return builder.createVariableInit(var, m_body->generateCode(builder));
}

GobLang::Codegen::BranchNode::BranchNode(
    std::unique_ptr<CodeNode> cond,
    std::unique_ptr<SequenceNode> body) : m_cond(std::move(cond)), m_body(std::move(body))
{
}

std::unique_ptr<GobLang::Codegen::BranchCodeGenValue> GobLang::Codegen::BranchNode::generateBranchCode(
    Builder &builder,
    size_t prevBranchOffset)
{
    std::vector<uint8_t> bytes = m_cond->generateCode(builder)->getGetOperationBytes();
    bytes.push_back((uint8_t)Operation::JumpIfNot);
    // pad the space to allocate space for future offset
    for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
    {
        bytes.push_back(0x0);
    }
    std::unique_ptr<BlockCodeGenValue> bodyContext = m_body->generateBlockContext(builder, bytes.size() + prevBranchOffset);
    std::vector<uint8_t> body = bodyContext->getGetOperationBytes();

    return std::make_unique<BranchCodeGenValue>(bytes, body);
}

std::string GobLang::Codegen::BranchNode::toString()
{
    return "{\"cond\": " + m_cond->toString() + ", \"body\": " + m_body->toString() + "}";
}

GobLang::Codegen::BranchChainNode::BranchChainNode(
    std::unique_ptr<BranchNode> primary,
    std::vector<std::unique_ptr<BranchNode>> secondary,
    std::unique_ptr<SequenceNode> elseBlock) : m_primary(std::move(primary)), m_secondary(std::move(secondary)), m_else(std::move(elseBlock))
{
}

std::string GobLang::Codegen::BranchChainNode::toString()
{
    std::string base = R"({"type" : "branch_chain",  "if": )" + m_primary->toString() + ", \"elifs\" : [";
    for (std::vector<std::unique_ptr<BranchNode>>::const_iterator it = m_secondary.begin(); it != m_secondary.end(); it++)
    {
        base.append((*it)->toString());
        if (it + 1 != m_secondary.end())
        {
            base += ',';
        }
    }
    base += "], \"else\" : ";
    if (m_else)
    {
        base += m_else->toString();
    }
    else
    {
        base += "null";
    }
    return base + "}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::BranchChainNode::generateCode(Builder &builder)
{
    //  where to jump if all blocks fail
    size_t endOffset = 0;

    std::unique_ptr<BranchCodeGenValue> ifBlock = m_primary->generateBranchCode(builder);
    if (!m_secondary.empty() || m_else)
    {
        ifBlock->addJump(0);
    }
    // head jumps just beyond this block if condition is false
    ifBlock->setConditionJumpOffset(ifBlock->getBodySize() + sizeof(ProgramAddressType) + 1);

    std::vector<std::unique_ptr<BranchCodeGenValue>> elifBlocks;
    // current size of the block + the future jump
    size_t prevChainSize = ifBlock->getFullSize(); // + sizeof(ProgramAddressType) + 1;
    for (std::vector<std::unique_ptr<BranchNode>>::const_iterator it = m_secondary.begin(); it != m_secondary.end(); it++)
    {
        elifBlocks.push_back((*it)->generateBranchCode(builder, prevChainSize));
        std::unique_ptr<GobLang::Codegen::BranchCodeGenValue> const &last = elifBlocks.back();
        if (it + 1 != m_secondary.end() || m_else)
        {
            last->addJump(0);
        }
        last->setConditionJumpOffset(last->getBodySize() + sizeof(ProgramAddressType) + 1);
        // skip past the entire block
        endOffset += last->getFullSize();
        prevChainSize += last->getFullSize();
        if (last->hasEndJump())
        {
            // prevChainSize += sizeof(ProgramAddressType);
        }
    }

    std::unique_ptr<BlockCodeGenValue> elseBlock = m_else ? m_else->generateBlockContext(
                                                                builder,
                                                                prevChainSize)
                                                          : nullptr;

    // primary block jumps to sizeof(self) + sum(...sizeof(elif)) + sizeof(else)

    if (elseBlock)
    {
        endOffset += elseBlock->getBlockSize();
    }
    ifBlock->addJump(endOffset + sizeof(ProgramAddressType) + 1);
    // but we don't update the value with the last block cause we are going to reuse it for writing proper elif jumps
    std::vector<uint8_t> bytes = ifBlock->getGetOperationBytes();
    for (std::unique_ptr<BranchCodeGenValue> &it : elifBlocks)
    {
        endOffset -= it->getFullSize();
        if (it->hasEndJump())
        {
            it->addJump(endOffset + sizeof(ProgramAddressType) + 1);
        }
        std::vector<uint8_t> temp = it->getGetOperationBytes();
        bytes.insert(bytes.end(), temp.begin(), temp.end());
    }
    std::vector<uint8_t> elseBlockBytes = elseBlock ? elseBlock->getGetOperationBytes() : std::vector<uint8_t>();
    bytes.insert(bytes.end(), elseBlockBytes.begin(), elseBlockBytes.end());

    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

GobLang::Codegen::BoolNode::BoolNode(bool val) : m_val(val)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::BoolNode::generateCode(Builder &builder)
{
    return builder.createConstBool(m_val);
}

std::string GobLang::Codegen::BoolNode::toString()
{
    return m_val ? "true" : "false";
}

GobLang::Codegen::WhileLoopNode::WhileLoopNode(std::unique_ptr<CodeNode> cond,
                                               std::unique_ptr<SequenceNode> body) : BranchNode(std::move(cond), std::move(body))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::WhileLoopNode::generateCode(Builder &builder)
{
    std::vector<uint8_t> bytes = getCond()->generateCode(builder)->getGetOperationBytes();
    bytes.push_back((uint8_t)Operation::JumpIfNot);
    // don't pad cause we can just insert value later

    std::unique_ptr<BlockCodeGenValue> bodyContext = getBody()->generateBlockContext(builder, 0, true);

    std::vector<uint8_t> body = bodyContext->getGetOperationBytes();
    std::vector<uint8_t> retNum = parseToBytes<ProgramAddressType>(body.size() + bytes.size() + sizeof(ProgramAddressType));
    // body always has a return, but it goes backwards and is equal to sizeof(body)
    body.push_back((uint8_t)Operation::JumpBack);

    body.insert(body.end(), retNum.begin(), retNum.end());

    // recalculate the size with the address included to properly jump forward
    retNum = parseToBytes<ProgramAddressType>(body.size() + sizeof(ProgramAddressType) + 1);
    bytes.insert(bytes.end(), retNum.begin(), retNum.end());
    size_t condSize = bytes.size();
    bytes.insert(bytes.end(), body.begin(), body.end());
    for (auto [addr, isBreak] : bodyContext->getBlock()->getJumps())
    {
        std::vector<uint8_t> jumpAddress;
        if (isBreak)
        {
            jumpAddress = parseToBytes<ProgramAddressType>(body.size() - addr);
        }
        else
        {

            jumpAddress = parseToBytes<ProgramAddressType>(condSize + addr);
        }
        std::copy(jumpAddress.begin(), jumpAddress.end(), bytes.begin() + addr + 1 + condSize);
        //   bytes.insert(bytes.begin() + addr + 1, jumpAddress.begin(), jumpAddress.end());
        std::cout << std::hex << addr + condSize << std::dec << std::endl;
    }
    return std::make_unique<GeneratedCodeGenValue>(bytes);
}

std::string GobLang::Codegen::WhileLoopNode::toString()
{
    return R"({"type": "while", "cond": )" + getCond()->toString() + ", \"body\": " + getBody()->toString() + "}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::BreakNode::generateCode(Builder &builder)
{
    BlockContext *block = builder.getCurrentBlock();
    if (block == nullptr)
    {
        throw ParsingError(0, 0, "Break used outside of a loop");
    }
    block->addJump(true);
    std::vector<uint8_t> bytes = {(uint8_t)Operation::Jump};
    for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
    {
        bytes.push_back(0);
    }
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::string GobLang::Codegen::BreakNode::toString()
{
    return std::string("\"break\"");
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::ContinueNode::generateCode(Builder &builder)
{
    BlockContext *block = builder.getCurrentBlock();
    if (block == nullptr)
    {
        throw ParsingError(0, 0, "Break used outside of a loop");
    }
    block->addJump(false);
    std::vector<uint8_t> bytes = {(uint8_t)Operation::JumpBack};
    for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
    {
        bytes.push_back(0);
    }
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::string GobLang::Codegen::ContinueNode::toString()
{
    return std::string("\"continue\"");
}

GobLang::Codegen::ArrayAccessNode::ArrayAccessNode(std::unique_ptr<CodeNode> value, std::unique_ptr<CodeNode> address)
    : m_array(std::move(value)), m_index(std::move(address))
{
}

std::string GobLang::Codegen::ArrayAccessNode::toString()
{
    return R"({"type" : "array_access", "val" :)" + m_array->toString() + ", \"addr\" : " + m_index->toString() + " }";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::ArrayAccessNode::generateCode(Builder &builder)
{

    return builder.createArrayAccess(m_array->generateCode(builder), m_index->generateCode(builder));
}

GobLang::Codegen::FunctionPrototypeNode::FunctionPrototypeNode(size_t nameId, std::vector<size_t> args) : m_nameId(nameId), m_argIds(std::move(args))
{
}

std::unique_ptr<GobLang::Codegen::FunctionPrototypeCodeGenValue> GobLang::Codegen::FunctionPrototypeNode::generateFunction(Builder &builder)
{
    return std::make_unique<FunctionPrototypeCodeGenValue>(builder.addFunction(m_nameId, m_argIds));
}

std::string GobLang::Codegen::FunctionPrototypeNode::toString()
{
    std::string str = R"({"type": "proto", "name" : )" + std::to_string(m_nameId) + ", \"args\" : [";
    for (std::vector<size_t>::const_iterator it = m_argIds.begin(); it != m_argIds.end(); it++)
    {
        str += std::to_string(*it);
        if (it + 1 != m_argIds.end())
        {
            str += ',';
        }
    }
    return str + "]}";
}

GobLang::Codegen::FunctionNode::FunctionNode(std::unique_ptr<FunctionPrototypeNode> proto,
                                             std::unique_ptr<CodeNode> body) : m_proto(std::move(proto)), m_body(std::move(body))
{
}

std::unique_ptr<GobLang::Codegen::FunctionCodeGenValue> GobLang::Codegen::FunctionNode::generateFunction(Builder &builder)
{
    std::unique_ptr<FunctionPrototypeCodeGenValue> func = m_proto->generateFunction(builder);
    builder.pushBlockForFunction(func->getFunc());
    BlockContext *body = builder.getCurrentBlock();
    std::vector<uint8_t> bodyBytes = m_body->generateCode(builder)->getGetOperationBytes();
    body->insert(bodyBytes);
    return std::make_unique<FunctionCodeGenValue>(func->getFunc(), builder.popBlock());
}

std::string GobLang::Codegen::FunctionNode::toString()
{
    return R"({"type":"function", "proto" : )" + m_proto->toString() + ", \"body\":" + m_body->toString() + "}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::ReturnEmptyNode::generateCode(Builder &builder)
{
    if (builder.isCurrentlyInFunction())
    {
        return std::make_unique<GeneratedCodeGenValue>(std::vector<uint8_t>{(uint8_t)Operation::Return});
    }
    return std::make_unique<GeneratedCodeGenValue>(std::vector<uint8_t>{(uint8_t)Operation::End});
}

std::string GobLang::Codegen::ReturnEmptyNode::toString()
{
    return R"({"type" : "ret"})";
}

GobLang::Codegen::ReturnNode::ReturnNode(std::unique_ptr<CodeNode> val) : m_val(std::move(val))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::ReturnNode::generateCode(Builder &builder)
{
    std::vector<uint8_t> bytes = m_val->generateCode(builder)->getGetOperationBytes();
    bytes.push_back((uint8_t)Operation::ReturnValue);
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::string GobLang::Codegen::ReturnNode::toString()
{
    return R"({"type" : "ret", "expr" : )" + m_val->toString() + "}";
}

GobLang::Codegen::CharacterNode::CharacterNode(char ch) : m_char(ch)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::CharacterNode::generateCode(Builder &builder)
{
    return builder.createConstChar(m_char);
}

std::string GobLang::Codegen::CharacterNode::toString()
{
    return R"({"ch" : ")" + std::string{m_char} + "\"}";
}

GobLang::Codegen::ArrayLiteralNode::ArrayLiteralNode(std::vector<std::unique_ptr<CodeNode>> values)
    : m_values(std::move(values))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::ArrayLiteralNode::generateCode(Builder &builder)
{
    std::vector<uint8_t> bytes;
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_values.begin(); it != m_values.end(); it++)
    {
        std::vector<uint8_t> temp = (*it)->generateCode(builder)->getGetOperationBytes();
        bytes.insert(bytes.end(), temp.begin(), temp.end());
    }
    bytes.push_back((uint8_t)Operation::CreateArray);
    bytes.push_back((uint8_t)m_values.size());
    return std::make_unique<GeneratedCodeGenValue>(std::move(bytes));
}

std::string GobLang::Codegen::ArrayLiteralNode::toString()
{
    std::string str = R"({"type":"array_literal", "values" : [)";
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_values.begin(); it != m_values.end(); it++)
    {
        str += (*it)->toString();
        if (it + 1 != m_values.end())
        {
            str += ',';
        }
    }
    return str + "]}";
}

GobLang::Codegen::UnaryOperationNode::UnaryOperationNode(Operator op, std::unique_ptr<CodeNode> value) : m_op(op), m_value(std::move(value))
{
}

std::string GobLang::Codegen::UnaryOperationNode::toString()
{
    const char *symb = std::find_if(Operators.begin(), Operators.end(), [this](OperatorData const &op)
                                    { return op.op == m_op; })
                           ->symbol;
    return R"({"type" : "unary", "value":)" + m_value->toString() + R"(, "op":")" + symb + "\"}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::UnaryOperationNode::generateCode(Builder &builder)
{
    return builder.createUnaryOperator(m_value->generateCode(builder), m_op);
}
