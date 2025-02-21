#include "Validator.hpp"
#include <iostream>
void GobLang::Compiler::Validator::validate()
{
    TokenIterator it = m_parser.getTokens().begin();
    while (code(it, it))
    {
        it++;
    }
    if (it != m_parser.getTokens().end())
    {
        throw ParsingError((*it)->getRow(), (*it)->getColumn(), "Unknown code sequence");
    }
}

bool GobLang::Compiler::Validator::constant(TokenIterator const &it)
{
    return it != getEnd() && (dynamic_cast<StringToken *>(*it) != nullptr ||
                              dynamic_cast<IntToken *>(*it) != nullptr ||
                              dynamic_cast<FloatToken *>(*it) != nullptr || 
                              dynamic_cast<CharToken *>(*it) != nullptr ||
                              dynamic_cast<BoolConstToken *>(*it) != nullptr ||
                              dynamic_cast<NullConstToken*>(*it) != nullptr);
}

bool GobLang::Compiler::Validator::id(TokenIterator const &it)
{
    return it != getEnd() && dynamic_cast<IdToken *>(*it) != nullptr;
}

bool GobLang::Compiler::Validator::unaryOperator(TokenIterator const &it)
{
    if (it == getEnd())
    {
        return false;
    }
    if (OperatorToken *op = dynamic_cast<OperatorToken *>(*it); op != nullptr)
    {
        return op->getOperator() == Operator::Not || op->getOperator() == Operator::Sub;
    }
    return false;
}

bool GobLang::Compiler::Validator::mathOperator(TokenIterator const &it)
{
    if (it == getEnd())
    {
        return false;
    }
    if (OperatorToken *op = dynamic_cast<OperatorToken *>(*it); op != nullptr && op->getOperator() != Operator::Assign)
    {
        return true;
    }
    return false;
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

bool GobLang::Compiler::Validator::loopControlKeyWord(TokenIterator const &it, TokenIterator &endIt)
{
    if (it == getEnd())
    {
        return false;
    }
    else if (KeywordToken *t = dynamic_cast<KeywordToken *>(*it);
             t != nullptr &&
             !(t->getKeyword() == Keyword::Break || t->getKeyword() == Keyword::Continue))
    {
        return false;
    }
    if (end(it + 1))
    {
        endIt = it + 1;
        return true;
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
    if (actionOperator(exprIt + 1, Operator::Assign))
    {
        throw ParsingError(getRowForToken(exprIt + 1), getColumnForToken(exprIt + 1), "Found '=' inside an expression, did you mean '=='?");
    }
    endIt = exprIt;
    return true;
}

bool GobLang::Compiler::Validator::unaryExpr(TokenIterator const &it, TokenIterator &endIt)
{
    return unaryOperator(it) && expr(it + 1, endIt);
}

bool GobLang::Compiler::Validator::mul(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if(arrayCreation(it, exprIt))
    {
        endIt = exprIt;
        return true;
    }
    if (arrayAccess(it, exprIt))
    {
        endIt = exprIt;
        return true;
    }
    if (call(it, exprIt))
    {
        endIt = exprIt;
        return true;
    }
    if (operand(it))
    {
        endIt = it;
        return true;
    }
    if (unaryExpr(it, exprIt))
    {
        endIt = exprIt;
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

bool GobLang::Compiler::Validator::call(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    if (!(id(it) && separator(it + 1, Separator::BracketOpen)))
    {
        return false;
    }
    exprIt++;
    do
    {
        exprIt++;
        if (separator(exprIt, Separator::BracketClose))
        {
            endIt = exprIt;
            // block ended after some about of operations
            return true;
        }
        else if (separator(exprIt, Separator::Comma))
        {
            exprIt++;
        }
    } while (expr(exprIt, exprIt) && (separator(exprIt + 1, Separator::Comma) || separator(exprIt + 1, Separator::BracketClose)));
    return false;
}

bool GobLang::Compiler::Validator::callOp(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    if (!call(it, exprIt))
    {
        return false;
    }
    if (!end(exprIt + 1))
    {
        throw ParsingError(getRowForToken(exprIt + 1), getColumnForToken(exprIt + 1), "Missing semicolon at the end of the expression");
        return false;
    }
    endIt = exprIt + 1;
    return true;
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

bool GobLang::Compiler::Validator::arrayCreation(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    if(!separator(exprIt, Separator::ArrayOpen))
    {
        return false;
    }
    do
    {
        exprIt++;
        if (separator(exprIt, Separator::ArrayClose))
        {
            endIt = exprIt;
            // block ended after some about of operations
            return true;
        }
        else if (separator(exprIt, Separator::Comma))
        {
            exprIt++;
        }
    } while (expr(exprIt, exprIt) && (separator(exprIt + 1, Separator::Comma) || separator(exprIt + 1, Separator::ArrayClose)));

    return false;
}

bool GobLang::Compiler::Validator::assignment(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt = it;
    if (!(id(it) && actionOperator(it + 1, Operator::Assign)))
    {
        return false;
    }
    if (!expr(it + 2, exprIt))
    {
        return false;
    }
    if (!end(exprIt + 1))
    {
        throw ParsingError(getRowForToken(exprIt + 1), getColumnForToken(exprIt + 1), "Missing semicolon at the end of the expression");
    }
    endIt = exprIt + 1;
    return true;
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
    if (!keyword(it, Keyword::Let))
    {
        return false;
    }
    bool valid = assignment(it + 1, exprIt);
    if (valid)
    {
        endIt = exprIt;
        return true;
    }
    return false;
}

bool GobLang::Compiler::Validator::returnOperation(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if (!keyword(it, Keyword::Return))
    {
        return false;
    }
    exprIt = it + 1;
    if (end(exprIt))
    {
        endIt = exprIt;
        return true;
    }
    if (!expr(exprIt, exprIt))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected an expression or ';'");
    }
    exprIt++;
    if (end(exprIt))
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
    } while (code(exprIt, exprIt));
    if (!separator(exprIt, Separator::BlockClose))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected assignment or a function call");
    }
    return false;
}

bool GobLang::Compiler::Validator::code(TokenIterator const &it, TokenIterator &endIt)
{
    return block(it, endIt) ||
           function(it, endIt) ||
           localVarCreation(it, endIt) ||
           arrayAssignment(it, endIt) ||
           assignment(it, endIt) ||
           callOp(it, endIt) ||
           ifElseChain(it, endIt) ||
           returnOperation(it, endIt) ||
           branch(BranchType::While, it, endIt) ||
           loopControlKeyWord(it, endIt);
}

bool GobLang::Compiler::Validator::branch(BranchType branch, TokenIterator const &it, TokenIterator &endIt)
{
    switch (branch)
    {
    case BranchType::If:
        if (!keyword(it, Keyword::If))
        {
            return false;
        }
        break;
    case BranchType::Elif:
        if (!keyword(it, Keyword::Elif))
        {
            return false;
        }
        break;
    case BranchType::Else:
        if (!keyword(it, Keyword::Else))
        {
            return false;
        }
        break;
    case BranchType::While:
        if (!keyword(it, Keyword::While))
        {
            return false;
        }
        break;
    default:
        return false;
    }

    TokenIterator exprIt = it + 1;
    if (branch != BranchType::Else)
    {
        if (!separator(exprIt, Separator::BracketOpen))
        {
            throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected '('");
        }
        if (!expr(exprIt, exprIt))
        {
            throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected condition");
        }
        if (!separator(exprIt, Separator::BracketClose))
        {
            throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected ')'");
        }
        exprIt++;
    }
    if (!block(exprIt, exprIt))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected  condition body");
    }
    endIt = exprIt;
    return true;
}

bool GobLang::Compiler::Validator::ifElseChain(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if (!branch(BranchType::If, it, exprIt))
    {
        return false;
    }
    while (branch(BranchType::Elif, exprIt + 1, exprIt))
    {
        // try as many of these as we can
    }
    branch(BranchType::Else, exprIt + 1, exprIt);
    endIt = exprIt;
    return true;
}

bool GobLang::Compiler::Validator::function(TokenIterator const &it, TokenIterator &endIt)
{
    TokenIterator exprIt;
    if (!keyword(it, Keyword::Function))
    {
        return false;
    }
    exprIt = it + 1;
    if (!id(exprIt))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected function name");
    }
    exprIt++;
    if (!separator(exprIt, Separator::BracketOpen))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected '('");
    }
    do
    {
        exprIt++;
        if (separator(exprIt, Separator::BracketClose))
        {

            exprIt++;
            break;
        }
        else if (separator(exprIt, Separator::Comma))
        {
            exprIt++;
        }
    } while (expr(exprIt, exprIt) && (separator(exprIt + 1, Separator::Comma) || separator(exprIt + 1, Separator::BracketClose)));

    if (!block(exprIt, exprIt))
    {
        throw ParsingError(getRowForToken(exprIt), getColumnForToken(exprIt), "Expected function body'");
    }
    endIt = exprIt;
    return true;
}

size_t GobLang::Compiler::Validator::getRowForToken(TokenIterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return m_parser.getTotalLineCount() - 1;
    }
    return (*it)->getRow();
}

size_t GobLang::Compiler::Validator::getColumnForToken(TokenIterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return m_parser.getFinalLineRowCount() - 1;
    }
    return (*it)->getColumn();
}
