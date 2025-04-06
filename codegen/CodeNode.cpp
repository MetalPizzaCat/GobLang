#include "CodeNode.hpp"
#include <algorithm>

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

std::string GobLang::Codegen::FloatNode::toString()
{
    return std::to_string(m_val);
}

GobLang::Codegen::IntNode::IntNode(int32_t val) : m_val(val)
{
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
