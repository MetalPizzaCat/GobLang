#include "Validator.hpp"
#include <iostream>
void GobLang::Compiler::Validator::validate()
{
    for (TokenIterator it = m_parser.getTokens().begin(); it != m_parser.getTokens().end(); it++)
    {
    }
}

bool GobLang::Compiler::Validator::constant(TokenIterator const &it)
{
    return it != getEnd() && (dynamic_cast<StringToken *>(*it) != nullptr ||
                              dynamic_cast<IntToken *>(*it) != nullptr ||
                              dynamic_cast<CharToken *>(*it) != nullptr ||
                              dynamic_cast<BoolConstToken *>(*it) != nullptr);
}

bool GobLang::Compiler::Validator::id(TokenIterator const &it)
{
    return it != getEnd() && dynamic_cast<IdToken *>(*it) != nullptr;
}

bool GobLang::Compiler::Validator::mathOperator(TokenIterator const &it)
{
    return it != getEnd() && dynamic_cast<OperatorToken *>(*it) != nullptr;
}

bool GobLang::Compiler::Validator::actionOperator(TokenIterator const &it, Operator op)
{
    if (it == getEnd())
    {
        return false;
    }
    else if (OperatorToken *t = dynamic_cast<OperatorToken *>(*it); t != nullptr)
    {
        return t->getOperator() == op;
    }
    return false;
}

bool GobLang::Compiler::Validator::separator(TokenIterator const &it, Separator sep)
{
    if (it == getEnd())
    {
        return false;
    }
    else if (SeparatorToken *t = dynamic_cast<SeparatorToken *>(*it); t != nullptr)
    {
        return t->getSeparator() == sep;
    }
    return false;
}

bool GobLang::Compiler::Validator::keyword(TokenIterator const &it, Keyword word)
{
    if (it == getEnd())
    {
        return false;
    }
    else if (KeywordToken *t = dynamic_cast<KeywordToken *>(*it); t != nullptr)
    {
        return t->getKeyword() == word;
    }
    return false;
}

bool GobLang::Compiler::Validator::end(TokenIterator const &it)
{
    return separator(it, Separator::End);
}

bool GobLang::Compiler::Validator::operand(TokenIterator const &it)
{
    return id(it) || constant(it);
}

bool GobLang::Compiler::Validator::expr(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if (!mul(it, exprIt))
    {
        return false;
    }
    while (mathOperator(exprIt + 1))
    {
        if (!mul(exprIt + 2, exprIt))
        {
            return false;
        }
    }
    endIt = exprIt;
    return true;
}

bool GobLang::Compiler::Validator::mul(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if (arrayAccess(it, exprIt))
    {
        endIt = exprIt;
        return true;
    }
    if (operand(it))
    {
        endIt = it;
        return true;
    }
    if (!separator(it, Separator::BracketOpen))
    {
        return false;
    }
    if (!expr(it + 1, exprIt))
    {
        return false;
    }
    if (!separator(exprIt + 1, Separator::BracketClose))
    {
        return false;
    }
    endIt = exprIt + 1;
    return true;

    return false;
}

bool GobLang::Compiler::Validator::groupedExpr(TokenIterator const &it, std::vector<Token *>::const_iterator &endIt)
{

    TokenIterator exprIt;
    bool valid = expr(it + 1, exprIt);
    if (!valid || !separator(exprIt, Separator::BracketOpen))
    {
        return false;
    }
    endIt = exprIt;
    return true;
}

bool GobLang::Compiler::Validator::arrayAccess(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    if (!id(it))
    {
        return false;
    }
    size_t depthCount = 0;
    while (arrayIndex(exprIt, exprIt))
    {
        depthCount++;
    }
    if (depthCount > 0)
    {
        endIt = exprIt;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::arrayIndex(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    bool valid =
        separator(exprIt + 1, Separator::ArrayOpen) &&
        expr(exprIt + 2, exprIt) &&
        separator(exprIt + 1, Separator::ArrayClose);
    if (valid)
    {
        endIt = exprIt + 1;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::assignment(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    bool valid = id(it) && actionOperator(it + 1, Operator::Assign) && expr(it + 2, exprIt) && end(exprIt + 1);
    if (valid)
    {
        endIt = exprIt + 1;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::arrayAssignment(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    bool valid = arrayAccess(it, exprIt) && actionOperator(exprIt + 1, Operator::Assign) && expr(exprIt + 2, exprIt) && end(exprIt + 1);
    if (valid)
    {
        endIt = exprIt + 1;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::localVarCreation(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    bool valid = keyword(it, Keyword::Let) && assignment(it + 1, exprIt);
    if (valid)
    {
        endIt = exprIt;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::block(TokenIterator const &it, TokenIterator &endIt)
{
    if (!separator(it, Separator::BlockOpen))
    {
        return false;
    }
    TokenIterator exprIt = it;
    do
    {
        exprIt++;
        if (separator(exprIt, Separator::BlockClose))
        {
            endIt = exprIt;
            // block ended after some about of operations
            return true;
        }
    } while (localVarCreation(exprIt, exprIt) || arrayAssignment(exprIt, exprIt) || assignment(exprIt, exprIt));
    return false;
}
