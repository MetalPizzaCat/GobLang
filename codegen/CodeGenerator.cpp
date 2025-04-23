#include "CodeGenerator.hpp"

using namespace GobLang::Codegen;

GobLang::Codegen::CodeGenerator::CodeGenerator(Parser const &parser) : m_parser(parser)
{
    m_it = parser.getTokens().begin();
}

void GobLang::Codegen::CodeGenerator::generate()
{
    while (isKeyword(Keyword::Function))
    {
        m_functions.push_back(parseFunctionDefinition());
    }
    m_rootSequence = parseBody();
}

ByteCode GobLang::Codegen::CodeGenerator::getByteCode()
{
    generate();
    Builder builder;
    ByteCode result;
    result.ids = m_parser.getIds();

    std::vector<uint8_t> funcBytes;
    for (std::vector<std::unique_ptr<FunctionNode>>::const_iterator it = m_functions.begin(); it != m_functions.end(); it++)
    {
        std::unique_ptr<FunctionCodeGenValue> func = (*it)->generateFunction(builder);
        std::vector<uint8_t> bytes = func->getGetOperationBytes();
        result.functions.push_back(*func->getFuncInfo());
        result.functions.back().start = funcBytes.size();

        funcBytes.insert(funcBytes.end(), bytes.begin(), bytes.end());
    }

    result.operations = m_rootSequence->generateCode(builder)->getGetOperationBytes();
    result.operations.push_back((uint8_t)Operation::End);

    for (std::vector<Function>::iterator it = result.functions.begin(); it != result.functions.end(); it++)
    {
        it->start += result.operations.size();
    }
    result.operations.insert(result.operations.end(), funcBytes.begin(), funcBytes.end());
    return result;
}

std::unique_ptr<FunctionNode> GobLang::Codegen::CodeGenerator::parseFunctionDefinition()
{
    consumeKeyword(Keyword::Function, "Expected 'func'");
    std::unique_ptr<FunctionPrototypeNode> proto = parseFunctionPrototype();
    consumeSeparator(Separator::BlockOpen, "Expected '{'");
    std::unique_ptr<SequenceNode> body = parseBody();
    if (!body)
    {
        error("Expected function body");
    }
    consumeSeparator(Separator::BlockClose, "Expected '}'");
    return std::make_unique<FunctionNode>(std::move(proto), std::move(body));
}

std::unique_ptr<FunctionPrototypeNode> GobLang::Codegen::CodeGenerator::parseFunctionPrototype()
{
    IdToken const *name = getTokenOrError<IdToken>("Expected function name");
    advance();
    consumeSeparator(Separator::BracketOpen, "Expected '('");
    std::vector<size_t> args;
    IdToken const *argTok = nullptr;
    while ((argTok = dynamic_cast<IdToken const *>(getCurrent())) != nullptr)
    {
        args.push_back(argTok->getId());
        advance();
        if (isSeparator(Separator::Comma))
        {
            advance();
        }
    }
    consumeSeparator(Separator::BracketClose, "Expected '('");
    return std::make_unique<FunctionPrototypeNode>(name->getId(), std::move(args));
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
        else if (isKeyword(Keyword::If))
        {
            seq.push_back(std::move(parseBranchChain()));
        }
        else if (isKeyword(Keyword::While))
        {
            seq.push_back(parseLoop());
        }
        else if (isKeyword(Keyword::Break))
        {
            seq.push_back(parseBreak());
        }
        else if (isKeyword(Keyword::Continue))
        {
            seq.push_back(parseContinue());
        }
        else if (isKeyword(Keyword::Return))
        {
            seq.push_back(parseReturn());
        }
        else if (isOfType<IdToken>())
        {
            seq.push_back(std::move(parseStandaloneExpression()));
        }
        else if (isSeparator(Separator::BlockClose) || isAtTheEnd())
        {
            break;
        }
        else
        {
            error("Unknown character sequence");
        }
        
    }
    return std::make_unique<SequenceNode>(std::move(seq));
}

std::unique_ptr<VariableCreationNode> GobLang::Codegen::CodeGenerator::parseVarCreation()
{
    advance();
    IdToken const *id = getTokenOrError<IdToken>("Expected variable name");
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

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseStandaloneExpression()
{
    IdToken const *t = getTokenOrError<IdToken>("Expected an identifier");
    advance();
    std::unique_ptr<CodeNode> expr = parseIdExpression(std::make_unique<IdNode>(t->getId()));
    if (isAssignment())
    {
        std::unique_ptr<CodeNode> val = parseBinaryOperationRightSide(0, std::move(expr));
        if (!val)
        {
            error("Expected a value");
        }
        consumeSeparator(Separator::End, "Expected ';'");
        return val;
    }
    consumeSeparator(Separator::End, "Expected ';'");
    return expr;
}

std::unique_ptr<ArrayLiteralNode> GobLang::Codegen::CodeGenerator::parseArrayLiteral()
{
    consumeSeparator(Separator::ArrayOpen, "Expected '['");
    if (isSeparator(Separator::ArrayClose))
    {
        advance();
        return std::make_unique<ArrayLiteralNode>();
    }
    std::vector<std::unique_ptr<CodeNode>> values;
    std::unique_ptr<CodeNode> val = nullptr;
    while ((val = parseExpression()) != nullptr)
    {
        values.push_back(std::move(val));
        if (isSeparator(Separator::Comma))
        {
            advance();
        }
        else if (isSeparator(Separator::ArrayClose))
        {
            break;
        }
        else
        {
            error("Expected ']' or ','");
        }
    }
    consumeSeparator(Separator::ArrayClose, "Expected ']'");
    return std::make_unique<ArrayLiteralNode>(std::move(values));
}

std::unique_ptr<BranchNode> GobLang::Codegen::CodeGenerator::parseBranch()
{
    // skip if
    advance();
    consumeSeparator(Separator::BracketOpen, "Expected '('");
    std::unique_ptr<CodeNode> cond = parseExpression();
    if (!cond)
    {
        error("Expected expression");
    }
    consumeSeparator(Separator::BracketClose, "Expected ')'");
    consumeSeparator(Separator::BlockOpen, "Expected '{'");
    std::unique_ptr<SequenceNode> body = parseBody();
    consumeSeparator(Separator::BlockClose, "Expected '}'");

    return std::make_unique<BranchNode>(std::move(cond), std::move(body));
}

std::unique_ptr<WhileLoopNode> GobLang::Codegen::CodeGenerator::parseLoop()
{
    // skip if
    advance();
    consumeSeparator(Separator::BracketOpen, "Expected '('");
    std::unique_ptr<CodeNode> cond = parseExpression();
    if (!cond)
    {
        error("Expected expression");
    }
    consumeSeparator(Separator::BracketClose, "Expected ')'");
    consumeSeparator(Separator::BlockOpen, "Expected '{'");
    std::unique_ptr<SequenceNode> body = parseBody();
    consumeSeparator(Separator::BlockClose, "Expected '}'");

    return std::make_unique<WhileLoopNode>(std::move(cond), std::move(body));
}

std::unique_ptr<BranchChainNode> GobLang::Codegen::CodeGenerator::parseBranchChain()
{
    std::unique_ptr<BranchNode> ifBlock = parseBranch();
    if (!ifBlock)
    {
        error("Expected condition block");
    }
    std::vector<std::unique_ptr<BranchNode>> elifs;
    while (isKeyword(Keyword::Elif))
    {
        elifs.push_back(std::move(parseBranch()));
    }
    std::unique_ptr<SequenceNode> elseBlock = nullptr;

    if (isKeyword(Keyword::Else))
    {
        advance();
        consumeSeparator(Separator::BlockOpen, "Expected '{'");
        elseBlock = parseBody();
        consumeSeparator(Separator::BlockClose, "Expected '}'");
    }
    return std::make_unique<BranchChainNode>(std::move(ifBlock), std::move(elifs), std::move(elseBlock));
}

std::unique_ptr<FloatNode> GobLang::Codegen::CodeGenerator::parseFloat()
{
    FloatToken const *t = getTokenOrError<FloatToken>("Expected a number");
    advance();
    return std::make_unique<FloatNode>(t->getValue());
}

std::unique_ptr<StringNode> GobLang::Codegen::CodeGenerator::parseString()
{
    StringToken const *t = getTokenOrError<StringToken>("Expected a string");
    advance();
    return std::make_unique<StringNode>(t->getId());
}

std::unique_ptr<IntNode> GobLang::Codegen::CodeGenerator::parseInt()
{
    IntToken const *t = getTokenOrError<IntToken>("Expected a number");
    advance();
    return std::make_unique<IntNode>(t->getValue());
}

std::unique_ptr<BoolNode> GobLang::Codegen::CodeGenerator::parseBool()
{
    BoolConstToken const *t = getTokenOrError<BoolConstToken>("Expected a bool value");
    advance();
    return std::make_unique<BoolNode>(t->getValue());
}

std::unique_ptr<CharacterNode> GobLang::Codegen::CodeGenerator::parseChar()
{
    CharToken const *t = getTokenOrError<CharToken>("Expected a character");
    advance();
    return std::make_unique<CharacterNode>(t->getChar());
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseId()
{
    IdToken const *t = getTokenOrError<IdToken>("Expected an identifier");
    std::unique_ptr<CodeNode> id = std::make_unique<IdNode>(t->getId());
    advance();
    return parseIdExpression(std::move(id));
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseIdExpression(std::unique_ptr<CodeNode> left)
{
    std::unique_ptr<CodeNode> value = std::move(left);
    while (true)
    {
        if (isSeparator(Separator::BracketOpen))
        {
            advance();
            std::vector<std::unique_ptr<CodeNode>> args = parseFunctionCallArguments();
            value = std::make_unique<FunctionCallNode>(std::move(value), std::move(args));
        }
        else if (isSeparator(Separator::ArrayOpen))
        {
            advance();
            std::unique_ptr<CodeNode> addr = parseArrayAccess();
            value = std::make_unique<ArrayAccessNode>(std::move(value), std::move(addr));
        }
        else
        {
            return value;
        }
    }
}

std::vector<std::unique_ptr<CodeNode>> GobLang::Codegen::CodeGenerator::parseFunctionCallArguments()
{
    if (isSeparator(Separator::BracketClose))
    {
        advance();
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
    consumeSeparator(Separator::BracketClose, "Expected ')'");
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
    else if (isOfType<StringToken>())
    {
        return parseString();
    }
    else if (isOfType<BoolConstToken>())
    {
        return parseBool();
    }
    else if (isOfType<CharToken>())
    {
        return parseChar();
    }
    else if (isSeparator(Separator::ArrayOpen))
    {
        return parseArrayLiteral();
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

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseArrayAccess()
{
    std::unique_ptr<CodeNode> addr = parseExpression();
    consumeSeparator(Separator::ArrayClose, "Expected ']'");
    return addr;
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

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseBreak()
{
    advance();
    consumeSeparator(Separator::End, "Expected ';'");
    return std::make_unique<BreakNode>();
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseContinue()
{
    advance();
    consumeSeparator(Separator::End, "Expected ';'");
    return std::make_unique<ContinueNode>();
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::parseReturn()
{
    advance();
    if (isSeparator(Separator::End))
    {
        advance();
        return std::make_unique<ReturnEmptyNode>();
    }
    std::unique_ptr<CodeNode> expr = parseExpression();
    consumeSeparator(Separator::End, "Expected ';'");
    return std::make_unique<ReturnNode>(std::move(expr));
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
    return nullptr;
}

int32_t GobLang::Codegen::CodeGenerator::getBinaryOperationPriority()
{
    return getCurrent() == nullptr ? -1 : getCurrent()->getPriority();
}

std::unique_ptr<CodeNode> GobLang::Codegen::CodeGenerator::error(std::string const &message)
{

    Token const *t = getCurrent();
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
    if (SeparatorToken const *sepTok = dynamic_cast<SeparatorToken const *>(getCurrent()); sepTok != nullptr && sepTok->getSeparator() == sep)
    {
        return true;
    }
    return false;
}

bool GobLang::Codegen::CodeGenerator::isKeyword(Keyword keyword)
{
    if (KeywordToken const *keyTok = dynamic_cast<KeywordToken const *>(getCurrent()); keyTok != nullptr && keyTok->getKeyword() == keyword)
    {
        return true;
    }
    return false;
}

bool GobLang::Codegen::CodeGenerator::isAssignment()
{
    if (OperatorToken const *opTok = dynamic_cast<OperatorToken const *>(getCurrent()); opTok != nullptr)
    {
        switch (opTok->getOperator())
        {
        case Operator::AddAssign:
        case Operator::SubAssign:
        case Operator::MulAssign:
        case Operator::DivAssign:
        case Operator::BitAndAssign:
        case Operator::BitNotAssign:
        case Operator::BitXorAssign:
        case Operator::ModuloAssign:
        case Operator::Assign:
            return true;
        default:
            return false;
        }
    }
    return false;
}

void GobLang::Codegen::CodeGenerator::printTree()
{
    if (m_rootSequence)
    {
        std::string out = "{\"strings\" : [";
        std::string strings;
        for (std::vector<std::string>::const_iterator it = m_parser.getIds().begin(); it != m_parser.getIds().end(); it++)
        {
            out += "\"" + (*it) + "\"";
            if (it + 1 != m_parser.getIds().end())
            {
                out += ",";
            }
        }
        out += "], \"functions\" : [";

        for (std::vector<std::unique_ptr<FunctionNode>>::const_iterator it = m_functions.begin(); it != m_functions.end(); it++)
        {
            out += (*it)->toString();
            if (it + 1 != m_functions.end())
            {
                out += ",";
            }
        }
        out += "], \"code\" : " + m_rootSequence->toString() + "}";
        std::cout << out << std::endl;
    }
}
