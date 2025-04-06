#include "CodeGenerator.hpp"

using namespace GobLang::Codegen;

GobLang::Codegen::CodeGenerator::CodeGenerator(Parser const &parser) : m_parser(parser)
{
    m_it = parser.getTokens().begin();
}

void GobLang::Codegen::CodeGenerator::generate()
{
    auto seq = parseBody();
    if (seq)
    {
        std::cout << seq->toString() << std::endl;
    }
}

std::unique_ptr<SequenceNode> GobLang::Codegen::CodeGenerator::parseBody()
{
    std::vector<std::unique_ptr<CodeNode>> seq;
    while (true)
    {
        if (isKeyword(Keyword::Let))
        {
            seq.push_back(std::move(parseVarCreation()));
        }
        if (isSeparator(Separator::BlockClose) || isAtTheEnd())
        {
            break;
        }
    }
    return std::make_unique<SequenceNode>(std::move(seq));
}

std::unique_ptr<VariableCreationNode> GobLang::Codegen::CodeGenerator::parseVarCreation()
{
    advance();
    IdToken *id = getTokenOrError<IdToken>("Expected variable name");
    advance();
    consumeOperator(Operator::Assign, "Expected '='");
    std::unique_ptr<CodeNode> body = parseExpression();
    if (!body)
    {
        error("Expected value");
    }
    consumeSeparator(Separator::End, "Expected ';'");
    return std::make_unique<VariableCreationNode>(id->getId(), std::move(body));
}

std::unique_ptr<FloatNode> GobLang::Codegen::CodeGenerator::parseFloat()
{
    if (FloatToken *t = getTokenOrError<FloatToken>("Expected a number"); t != nullptr)
    {
        advance();
        return std::make_unique<FloatNode>(t->getValue());
    }
    return nullptr;
}

std::unique_ptr<IntNode> GobLang::Codegen::CodeGenerator::parseInt()
{
    if (IntToken *t = getTokenOrError<IntToken>("Expected a number"); t != nullptr)
    {
        advance();
        return std::make_unique<IntNode>(t->getValue());
    }
    return nullptr;
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseId()
{
    IdToken *t = getTokenOrError<IdToken>("Expected an identifier");
    advance();
    if (isSeparator(Separator::BracketOpen))
    {
        advance();
        return std::make_unique<FunctionCallNode>(t->getId(), parseFunctionCallArguments());
    }
    else if (isSeparator(Separator::ArrayOpen))
    {
        error("Add array access parsing");
    }
    else
    {
        return std::make_unique<IdNode>(t->getId());
    }
    return nullptr;
}

std::vector<std::unique_ptr<CodeNode>> GobLang::Codegen::CodeGenerator::parseFunctionCallArguments()
{
    if (isSeparator(Separator::BracketClose))
    {
        return {};
    }
    std::vector<std::unique_ptr<CodeNode>> args;
    while (true)
    {
        if (std::unique_ptr<CodeNode> arg = parseExpression())
        {
            args.push_back(std::move(arg));
        }
        else
        {
            return {};
        }
        if (isSeparator(Separator::BracketClose))
        {
            break;
        }
        consumeSeparator(Separator::Comma, "Expected ', or ')'");
    }
    // consume the ')'
    advance();
    return args;
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parsePrimary()
{
    if (isOfType<FloatToken>())
    {
        return parseFloat();
    }
    else if (isOfType<IdToken>())
    {
        return parseId();
    }
    else if (isOfType<IntToken>())
    {
        return parseInt();
    }
    else if (isSeparator(Separator::BracketOpen))
    {
        return parseGrouped();
    }
    else
    {
        error("Unexpected character sequence");
    }
    return nullptr;
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseGrouped()
{
    advance();
    std::unique_ptr<CodeNode> v = parseExpression();
    if (!v)
    {
        return nullptr;
    }
    if (isSeparator(Separator::BracketClose))
    {
        advance();
        return v;
    }
    return nullptr;
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseExpression()
{
    std::unique_ptr<CodeNode> left = parsePrimary();
    if (!left)
    {
        return nullptr;
    }
    return parseBinaryOperationRightSide(0, std::move(left));
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseBinaryOperationRightSide(int32_t priority, std::unique_ptr<CodeNode> leftSide)
{
    std::unique_ptr<CodeNode> left(std::move(leftSide));
    while (true)
    {
        int32_t tokPriority = getBinaryOperationPriority();
        if (tokPriority < priority)
        {
            return left;
        }
        Operator op = getTokenOrError<OperatorToken>("Expected an operator")->getOperator();
        advance();
        std::unique_ptr<CodeNode> right = parsePrimary();
        if (!right)
        {
            return nullptr;
        }
        int32_t nextPriority = getBinaryOperationPriority();
        if (tokPriority < nextPriority)
        {
            right = parseBinaryOperationRightSide(tokPriority + 1, std::move(right));
            if (!right)
            {
                return nullptr;
            }
        }
        left = std::make_unique<BinaryOperationNode>(op, std::move(left), std::move(right));
    }
    return std::unique_ptr<CodeNode>();
}

int32_t GobLang::Codegen::CodeGenerator::getBinaryOperationPriority()
{
    return getCurrent() == nullptr ? -1 : getCurrent()->getPriority();
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::error(std::string const &message)
{

    Token *t = getCurrent();
    if (t == nullptr)
    {
        t = (m_it - 1)->get();
    }
    throw ParsingError(t->getRow(), t->getColumn(), message);
    return nullptr;
}

void GobLang::Codegen::CodeGenerator::consumeSeparator(Separator sep, std::string const &err)
{
    if (getTokenOrError<SeparatorToken>(err)->getSeparator() != sep)
    {
        error(err);
    }
    advance();
}

void GobLang::Codegen::CodeGenerator::consumeKeyword(Keyword keyword, std::string const &err)
{
    if (getTokenOrError<KeywordToken>(err)->getKeyword() != keyword)
    {
        error(err);
    }
    advance();
}

void GobLang::Codegen::CodeGenerator::consumeOperator(Operator op, std::string const &err)
{
    if (getTokenOrError<OperatorToken>(err)->getOperator() != op)
    {
        error(err);
    }
    advance();
}

bool GobLang::Codegen::CodeGenerator::isSeparator(Separator sep)
{
    if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(getCurrent()); sepTok != nullptr && sepTok->getSeparator() == sep)
    {
        return true;
    }
    return false;
}

bool GobLang::Codegen::CodeGenerator::isKeyword(Keyword keyword)
{
    if (KeywordToken *keyTok = dynamic_cast<KeywordToken *>(getCurrent()); keyTok != nullptr && keyTok->getKeyword() == keyword)
    {
        return true;
    }
    return false;
}
