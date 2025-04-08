#include "CodeNode.hpp"
#include <algorithm>

#include "Lexems.hpp"
#include "../execution/Value.hpp"
GobLang::Codegen::IdNode::IdNode(size_t id) : m_id(id)
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::IdNode::generateCode(Builder &builder)
{
    return builder.createVariableAccess(m_id);
}

std::string GobLang::Codegen::IdNode::toString()
{
    return "{\"id\": \"" + std::to_string(m_id) + "\"}";
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
    return "{\"str\": \"" + std::to_string(m_id) + "\"}";
}

GobLang::Codegen::SequenceNode::SequenceNode(std::vector<std::unique_ptr<CodeNode>> seq) : m_sequence(std::move(seq))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::SequenceNode::generateCode(Builder &builder)
{
    builder.pushEmptyBlock();
    BlockContext *block = builder.getCurrentBlock();
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_sequence.begin(); it != m_sequence.end(); it++)
    {
        block->insert((*it)->generateCode(builder)->getGetOperationBytes());
    }
    block->appendMemoryClear();
    return std::make_unique<BlockCodeGenValue>(builder.popBlock());
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
    return "{ \"left\":" + m_left->toString() + ", \"right\": " + m_right->toString() + ", \"op\":\"" + symb + "\"}";
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
        throw std::exception();
    // handle assignment first
    case Operator::Assign:
        return builder.createAssignment(m_left->generateCode(builder), m_right->generateCode(builder), m_op);
    default:
        return builder.createOperation(m_left->generateCode(builder), m_right->generateCode(builder), m_op);
    }
}

GobLang::Codegen::FunctionCallNode::FunctionCallNode(size_t id, std::vector<std::unique_ptr<CodeNode>> args) : m_id(id), m_args(std::move(args))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::FunctionCallNode::generateCode(Builder &builder)
{
    std::vector<std::unique_ptr<CodeGenValue>> args;
    for (std::vector<std::unique_ptr<CodeNode>>::const_iterator it = m_args.begin(); it != m_args.end(); it++)
    {
        args.push_back(std::move((*it)->generateCode(builder)));
    }
    return builder.createCall(m_id, std::move(args));
}

std::string GobLang::Codegen::FunctionCallNode::toString()
{
    std::string base = "{ \"type\" : \"call\", \"name\":\"" + std::to_string(m_id) + "\", \"args\": [";
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
    return "{\"type\" : \"let\", \"var\": " + std::to_string(m_id) + ", \"body\" : " + m_body->toString() + "}";
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::VariableCreationNode::generateCode(Builder &builder)
{
    size_t var = builder.insertVariable(m_id);
    if (var == -1)
    {
        throw std::exception();
    }
    return builder.createVariableInit(var, m_body->generateCode(builder));
}

GobLang::Codegen::BranchNode::BranchNode(std::unique_ptr<CodeNode> cond, std::unique_ptr<CodeNode> body) : m_cond(std::move(cond)), m_body(std::move(body))
{
}

std::unique_ptr<GobLang::Codegen::BranchCodeGenValue> GobLang::Codegen::BranchNode::generateBranchCode(Builder &builder)
{
    std::vector<uint8_t> bytes = m_cond->generateCode(builder)->getGetOperationBytes();
    bytes.push_back((uint8_t)Operation::JumpIfNot);
    // pad the space to allocate space for future offset
    for (size_t i = 0; i < sizeof(ProgramAddressType); i++)
    {
        bytes.push_back(0x0);
    }
    std::vector<uint8_t> body = m_body->generateCode(builder)->getGetOperationBytes();

    return std::make_unique<BranchCodeGenValue>(bytes, body);
}

std::string GobLang::Codegen::BranchNode::toString()
{
    return "{\"cond\": " + m_cond->toString() + ", \"body\": " + m_body->toString() + "}";
}

GobLang::Codegen::BranchChainNode::BranchChainNode(
    std::unique_ptr<BranchNode> primary,
    std::vector<std::unique_ptr<BranchNode>> secondary,
    std::unique_ptr<CodeNode> elseBlock) : m_primary(std::move(primary)), m_secondary(std::move(secondary)), m_else(std::move(elseBlock))
{
}

std::string GobLang::Codegen::BranchChainNode::toString()
{
    std::string base = "{ \"if\": " + m_primary->toString() + ", \"elifs\" : [";
    for (std::vector<std::unique_ptr<BranchNode>>::const_iterator it = m_secondary.begin(); it != m_secondary.end(); it++)
    {
        base += (*it)->toString();
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
    BlockContext *block = builder.getCurrentBlock();
    // where to jump if all blocks fail
    size_t endOffset = 0;

    std::unique_ptr<BranchCodeGenValue> ifBlock = m_primary->generateBranchCode(builder);
    if (m_secondary.empty() || m_else)
    {
        ifBlock->addJump(0);
    }
    // head jumps just beyond this block if condition is false
    ifBlock->setConditionJumpOffset(ifBlock->getBodySize() + sizeof(ProgramAddressType) + 1);

    std::vector<std::unique_ptr<BranchCodeGenValue>> elifBlocks;
    for (std::vector<std::unique_ptr<BranchNode>>::const_iterator it = m_secondary.begin(); it != m_secondary.end(); it++)
    {
        elifBlocks.push_back((*it)->generateBranchCode(builder));

        if (it + 1 != m_secondary.end() || m_else)
        {
            elifBlocks.back()->addJump(0);
        }
        elifBlocks.back()->setConditionJumpOffset(elifBlocks.back()->getBodySize() + sizeof(ProgramAddressType) + 1);
        // skip past the entire block
        endOffset += elifBlocks.back()->getFullSize();
    }

    std::vector<uint8_t> elseBlock = m_else ? m_else->generateCode(builder)->getGetOperationBytes() : std::vector<uint8_t>();

    // primary block jumps to sizeof(self) + sum(...sizeof(elif)) + sizeof(else)

    endOffset += elseBlock.size();
    ifBlock->addJump(endOffset + sizeof(ProgramAddressType) + 1);
    // but we don't update the value with the last block cause we are going to reuse it for writing proper elif jumps
    std::vector<uint8_t> bytes = ifBlock->getGetOperationBytes();
    for (std::vector<std::unique_ptr<BranchCodeGenValue>>::const_iterator it = elifBlocks.begin(); it != elifBlocks.end(); it++)
    {
        endOffset -= (*it)->getFullSize();
        if ((*it)->hasEndJump())
        {
            (*it)->addJump(endOffset + sizeof(ProgramAddressType) + 1);
        }
        std::vector<uint8_t> temp = (*it)->getGetOperationBytes();
        bytes.insert(bytes.end(), temp.begin(), temp.end());
    }
    bytes.insert(bytes.end(), elseBlock.begin(), elseBlock.end());

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

GobLang::Codegen::WhileLoopNode::WhileLoopNode(std::unique_ptr<CodeNode> cond, std::unique_ptr<CodeNode> body) : BranchNode(std::move(cond), std::move(body))
{
}

std::unique_ptr<GobLang::Codegen::CodeGenValue> GobLang::Codegen::WhileLoopNode::generateCode(Builder &builder)
{
    BlockContext *block = builder.getCurrentBlock();
    std::vector<uint8_t> bytes = m_cond->generateCode(builder)->getGetOperationBytes();
    bytes.push_back((uint8_t)Operation::JumpIfNot);
    // don't pad cause we can just insert value later
    
    std::vector<uint8_t> body = m_body->generateCode(builder)->getGetOperationBytes();
    std::vector<uint8_t> retNum = parseToBytes(body.size() + bytes.size() + sizeof(ProgramAddressType));
    // body always has a return, but it goes backwards and is equal to sizeof(body)
    body.push_back((uint8_t)Operation::JumpBack);
   
    body.insert(body.end(), retNum.begin(), retNum.end());

    // recalculate the size with the address included to properly jump forward
    retNum = parseToBytes(body.size() + sizeof(ProgramAddressType) + 1);
    bytes.insert(bytes.end(), retNum.begin(), retNum.end());
    bytes.insert(bytes.end(), body.begin(), body.end());

    return std::make_unique<GeneratedCodeGenValue>(bytes);
}
