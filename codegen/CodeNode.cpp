#include "CodeNode.hpp"
#include <algorithm>

#include "Lexems.hpp"
GobLang::Codegen::IdNode::IdNode(size_t id) : m_id(id)
{
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
    // handle assignment first
    case Operator::Assign:
    case Operator::AddAssign:
    case Operator::SubAssign:
    case Operator::MulAssign:
    case Operator::DivAssign:
    case Operator::ModuloAssign:
    {
    }
    break;
    default:
    {
        return builder.createFloatOperation(m_left->generateCode(builder), m_right->generateCode(builder), m_op);
    }
    break;
    }
    return std::unique_ptr<CodeGenValue>();
}

GobLang::Codegen::FunctionCallNode::FunctionCallNode(size_t id, std::vector<std::unique_ptr<CodeNode>> args) : m_id(id), m_args(std::move(args))
{
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
