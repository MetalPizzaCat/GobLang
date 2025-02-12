#include "ReversePolishGenerator.hpp"

void GobLang::Compiler::ReversePolishGenerator::compile()
{
    for (m_it = m_parser.getTokens().begin(); m_it != m_parser.getTokens().end(); m_it++)
    {
        if (IdToken *id = dynamic_cast<IdToken *>(*m_it); id != nullptr)
        {
            if (m_isVariableDeclaration)
            {
                _appendVariable(id->getId());
                LocalVarToken *local = new LocalVarToken(id->getRow(), id->getColumn(), _getLocalVariableAccessId(id->getId()));
                m_code.push_back(local);
                m_compilerTokens.push_back(local);
                m_isVariableDeclaration = false;
            }
            else if (int32_t varId = _getLocalVariableAccessId(id->getId()); varId != -1)
            {
                LocalVarToken *local = new LocalVarToken(id->getRow(), id->getColumn(), varId);
                m_code.push_back(local);
                m_compilerTokens.push_back(local);
            }
            else
            {
                m_code.push_back(*m_it);
            }
        }
        else if (dynamic_cast<IntToken *>(*m_it) != nullptr ||
                 dynamic_cast<StringToken *>(*m_it) != nullptr ||
                 dynamic_cast<BoolConstToken *>(*m_it) != nullptr ||
                 dynamic_cast<CharToken *>(*m_it) != nullptr)
        {
            m_code.push_back(*m_it);
            continue;
        }
        else if (KeywordToken *keyTok = dynamic_cast<KeywordToken *>(*m_it); keyTok != nullptr)
        {
            _compileKeywords(keyTok, m_it);
        }
        else if (SeparatorToken *sepToken = dynamic_cast<SeparatorToken *>(*m_it); sepToken != nullptr)
        {
            _compileSeparators(sepToken, m_it);
        }
        else if (dynamic_cast<OperatorToken *>(*m_it) != nullptr)
        {
            if ((*m_it)->getPriority() > getTopStackPriority())
            {
                _addOperator(m_it);
                continue;
            }
            while (!m_stack.empty() && getTopStackPriority() >= (*m_it)->getPriority())
            {
                m_code.push_back(popStack());
            }
            _addOperator(m_it);
        }
    }
    dumpStack();
}

void GobLang::Compiler::ReversePolishGenerator::dumpStack()
{
    // dump the remaining stack
    for (std::vector<Token *>::const_reverse_iterator it = m_stack.rbegin(); it != m_stack.rend(); it++)
    {
        m_code.push_back(*it);
    }
    m_stack.clear();
}

void GobLang::Compiler::ReversePolishGenerator::dumpStackWhile(std::function<bool(Token *)> const &cond)
{
    for (int32_t i = m_stack.size() - 1; i >= 0 && cond(m_stack[i]); i--)
    {
        m_code.push_back(m_stack[i]);
        m_stack.pop_back();
    }
}

int32_t GobLang::Compiler::ReversePolishGenerator::getTopStackPriority()
{
    if (m_stack.empty())
    {
        return -1;
    }
    return m_stack[m_stack.size() - 1]->getPriority();
}

GobLang::Compiler::Token *GobLang::Compiler::ReversePolishGenerator::popStack()
{
    if (m_stack.empty())
    {
        return nullptr;
    }
    Token *tkn = m_stack[m_stack.size() - 1];
    m_stack.pop_back();
    return tkn;
}

void GobLang::Compiler::ReversePolishGenerator::printCode()
{
    for (std::vector<Token *>::iterator it = m_code.begin(); it != m_code.end(); it++)
    {
        std::cout << (*it)->toString() << " ";
    }
    std::cout << std::endl;
}

void GobLang::Compiler::ReversePolishGenerator::printFunctions()
{
    for (std::vector<FunctionTokenSequence *>::iterator it = m_funcs.begin(); it != m_funcs.end(); it++)
    {
        std::cout << "FUNC_" << (*it)->getInfo()->nameId << "(";
        for (std::vector<FunctionArgInfo>::const_iterator argIt = (*it)->getInfo()->arguments.begin(); argIt != (*it)->getInfo()->arguments.end(); argIt++)
        {
            std::cout << "ARG" << argIt->nameId << ",";
        }
        std::cout << ")";
        for (std::vector<Token *>::const_iterator tokIt = (*it)->getTokens().begin(); tokIt != (*it)->getTokens().end(); tokIt++)
        {
            std::cout << (*tokIt)->toString() << " " << std::endl;
        }
    }
}

void GobLang::Compiler::ReversePolishGenerator::addToken(Token *token)
{
    if(m_currentFunction == nullptr)
    {
        m_code.push_back(token);
    }
    else{
        m_currentFunction->addToken(token);
    }
}

GobLang::Compiler::WhileToken *GobLang::Compiler::ReversePolishGenerator::getCurrentLoop()
{
    for (std::vector<GotoToken *>::reverse_iterator it = m_jumps.rbegin(); it != m_jumps.rend(); it++)
    {
        if (WhileToken *tok = dynamic_cast<WhileToken *>(*it); tok != nullptr)
        {
            return tok;
        }
    }
    return nullptr;
}

size_t GobLang::Compiler::ReversePolishGenerator::getMarkCounterAndAdvance()
{
    return m_markCounter++;
}

bool GobLang::Compiler::ReversePolishGenerator::_doesVariableExist(size_t stringId)
{
    for (std::vector<std::vector<size_t>>::iterator it = m_blockVariables.begin(); it != m_blockVariables.end(); it++)
    {
        if (std::find(it->begin(), it->end(), stringId) != it->end())
        {
            return true;
        }
    }
    return false;
}

int32_t GobLang::Compiler::ReversePolishGenerator::_getLocalVariableAccessId(size_t id)
{
    size_t curr = 0;
    bool found = false;
    for (std::vector<std::vector<size_t>>::reverse_iterator it = m_blockVariables.rbegin(); it != m_blockVariables.rend(); it++)
    {
        if (found)
        {
            curr += it->size();
        }
        else if (std::vector<size_t>::iterator localIt = std::find(it->begin(), it->end(), id); localIt != it->end())
        {
            found = true;
            curr = localIt - it->begin();
        }
    }
    return found ? (int32_t)curr : -1;
}

void GobLang::Compiler::ReversePolishGenerator::_appendVariableBlock()
{
    m_blockVariables.push_back({});
}

void GobLang::Compiler::ReversePolishGenerator::_popVariableBlock()
{
    m_blockVariables.pop_back();
}

void GobLang::Compiler::ReversePolishGenerator::_appendVariable(size_t stringId)
{
    m_blockVariables.rbegin()->push_back(stringId);
}

GobLang::Compiler::ReversePolishGenerator::~ReversePolishGenerator()
{
    for (size_t i = 0; i < m_compilerTokens.size(); i++)
    {
        delete m_compilerTokens[i];
    }

    for (size_t i = 0; i < m_funcs.size(); i++)
    {
        delete m_funcs[i];
    }
}

void GobLang::Compiler::ReversePolishGenerator::_compileSeparators(SeparatorToken *sepToken, std::vector<Token *>::const_iterator const &it)
{

    switch (sepToken->getSeparator())
    {
    case Separator::End:
        dumpStack();
        m_code.push_back(sepToken);
        break;
    case Separator::BracketOpen:
        if (dynamic_cast<IdToken *>(*(it - 1)) != nullptr)
        {
            FunctionCallToken *token = new FunctionCallToken(sepToken->getRow(), sepToken->getColumn());
            m_compilerTokens.push_back(token);
            m_stack.push_back(token);
            m_functionCalls.push_back(token);
        }
        else if (!_isBranchKeyword(it - 1))
        {
            m_stack.push_back(*it);
        }

        break;
    case Separator::Comma:
        if (!m_functionCalls.empty())
        {
            dumpStackWhile([](Token *t)
                           { return dynamic_cast<FunctionCallToken *>(t) == nullptr; });
            (*m_functionCalls.rbegin())->increaseArgCount();
        }
        break;
    case Separator::ArrayOpen:
    {
        ArrayIndexToken *token = new ArrayIndexToken(sepToken->getRow(), sepToken->getColumn());
        m_compilerTokens.push_back(token);
        m_stack.push_back(token);
    }
    break;
    case Separator::ArrayClose:

        if (m_stack.empty())
        {
            throw ParsingError(sepToken->getRow(), sepToken->getColumn(), "Encountered ')' without previous '('");
        }
        else
        {
            Token *arrayPopToken = nullptr;
            do
            {
                arrayPopToken = *(m_stack.rbegin());
                m_code.push_back(arrayPopToken);
                m_stack.pop_back();
            } while (!m_stack.empty() && dynamic_cast<ArrayIndexToken *>(arrayPopToken) == nullptr);
        }
        break;

    case Separator::BracketClose:
        //_printTokenStack();
        while (!m_stack.empty())
        {
            Token *t = *(m_stack.rbegin());
            m_stack.pop_back();
            if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(t); sepTok != nullptr && sepTok->getSeparator() == Separator::BracketOpen)
            {
                break;
            }
            else if (FunctionCallToken *funcTok = dynamic_cast<FunctionCallToken *>(t); funcTok != nullptr)
            {
                SeparatorToken *prev = dynamic_cast<SeparatorToken *>(*(it - 1));
                if (!(prev != nullptr && (prev->getSeparator() == Separator::BracketOpen || prev->getSeparator() == Separator::Comma)))
                {
                    (*m_functionCalls.rbegin())->increaseArgCount();
                }
                m_code.push_back(*m_functionCalls.rbegin());
                m_functionCalls.pop_back();
                break;
            }
            else if (dynamic_cast<IfToken *>(t) != nullptr || dynamic_cast<WhileToken *>(t) != nullptr)
            {
                m_code.push_back(t);
                break;
            }
            else
            {
                m_code.push_back(t);
            }
        }
        break;
    case Separator::BlockOpen:
        _appendVariableBlock();
        break;
    case Separator::BlockClose:
        dumpStack();
        if (m_blockVariables.rbegin()->size() > 0)
        {
            m_code.push_back(new LocalVarShrinkToken(sepToken->getRow(), sepToken->getColumn(), m_blockVariables.rbegin()->size()));
        }
        _popVariableBlock();
        if (!m_jumps.empty())
        {
            GotoToken *jump = *m_jumps.rbegin();
            m_jumps.pop_back();
            if (IfToken *ifToken = dynamic_cast<IfToken *>(jump); ifToken != nullptr && ifToken->isElif())
            {
                if (m_jumps.empty())
                {
                    throw ParsingError(ifToken->getRow(), ifToken->getColumn(), "Invalid elif configuration, jump token was not generated");
                }
                GotoToken *elifPair = *m_jumps.rbegin();
                elifPair->setMark(getMarkCounterAndAdvance());
                JumpDestinationToken *dest = new JumpDestinationToken(sepToken->getRow(), sepToken->getColumn(), elifPair->getMark());
                m_jumps.pop_back();
                m_compilerTokens.push_back(dest);
                m_code.push_back(dest);
            }

            else if (WhileToken *whileToken = dynamic_cast<WhileToken *>(jump); whileToken != nullptr)
            {
                whileToken->setReturnMark(getMarkCounterAndAdvance());
                GotoToken *loopJump = new GotoToken(sepToken->getRow(), sepToken->getColumn(), whileToken->getReturnMark());
                m_compilerTokens.push_back(loopJump);
                m_code.push_back(loopJump);
            }
            jump->setMark(getMarkCounterAndAdvance());
            JumpDestinationToken *dest = new JumpDestinationToken(sepToken->getRow(), sepToken->getColumn(), jump->getMark());
            if (_isElseChainToken(it + 1) || _isElifChainToken(it + 1))
            {
                GotoToken *elseJump = new GotoToken(sepToken->getRow(), sepToken->getColumn());
                m_jumps.push_back(elseJump);
                m_compilerTokens.push_back(elseJump);
                m_code.push_back(elseJump);
            }

            m_compilerTokens.push_back(dest);
            m_code.push_back(dest);
        }
        break;
    }
}

void GobLang::Compiler::ReversePolishGenerator::_compileKeywords(KeywordToken *keyToken, std::vector<Token *>::const_iterator const &it)
{
    switch (keyToken->getKeyword())
    {
    case Keyword::Elif:
    case Keyword::If:
    {
        if (it + 1 == m_parser.getTokens().end() || dynamic_cast<SeparatorToken *>(*(it + 1)) == nullptr)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Missing condition for 'if' construct");
        }
        else if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*(it + 1)); sepTok != nullptr && sepTok->getSeparator() != Separator::BracketOpen)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Missing condition for 'if' construct");
        }
        IfToken *ifTok = new IfToken(keyToken->getRow(), keyToken->getColumn(), keyToken->getKeyword() == Keyword::Elif);
        m_stack.push_back(ifTok);
        m_jumps.push_back(ifTok);
        m_compilerTokens.push_back(ifTok);
        m_isInConditionHead = true;
    }
    break;
    case Keyword::While:
    {
        WhileToken *whileTok = new WhileToken(keyToken->getRow(), keyToken->getColumn());
        m_stack.push_back(whileTok);
        m_jumps.push_back(whileTok);
        m_compilerTokens.push_back(whileTok);
        m_isInConditionHead = true;
    }
    break;
    case Keyword::Else:
        if (it + 1 == m_parser.getTokens().end() || dynamic_cast<SeparatorToken *>(*(it + 1)) == nullptr)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Else construct is missing body block");
        }
        else if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*(it + 1)); sepTok != nullptr && sepTok->getSeparator() != Separator::BlockOpen)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Else keyword must be followed by a code block");
        }

        break;
    case Keyword::Let:
        if (it + 1 == m_parser.getTokens().end() || dynamic_cast<IdToken *>(*(it + 1)) == nullptr)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Missing variable name in variable declaration");
        }
        m_isVariableDeclaration = true;
        break;
    case Keyword::Continue:

        if (WhileToken *whileTok = getCurrentLoop(); whileTok != nullptr)
        {
            LoopControlToken *contTok = new LoopControlToken(keyToken->getRow(), keyToken->getColumn(), false, whileTok);
            m_compilerTokens.push_back(contTok);
            m_code.push_back(contTok);
        }
        break;
    case Keyword::Break:
        if (WhileToken *whileTok = getCurrentLoop(); whileTok != nullptr)
        {
            LoopControlToken *contTok = new LoopControlToken(keyToken->getRow(), keyToken->getColumn(), true, whileTok);
            m_compilerTokens.push_back(contTok);
            m_code.push_back(contTok);
        }
        break;
    case Keyword::Function:
        // functions have their own way of being parsed to avoid messing with the header
        _compileFunction(it, m_it);
        break;
    default:
        throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Invalid keyword encountered");
        break;
    }
}

void GobLang::Compiler::ReversePolishGenerator::_compileFunction(
    std::vector<Token *>::const_iterator const &start,
    std::vector<Token *>::const_iterator &end)
{
    Function *func = new Function();
    std::vector<Token *>::const_iterator currIt = start + 1;
    if (IdToken *funcNameTok = dynamic_cast<IdToken *>(*currIt); funcNameTok != nullptr)
    {
        func->nameId = funcNameTok->getId();
    }
    else
    {
        throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Missing function name");
    }
    currIt++;
    if (currIt == m_parser.getTokens().end())
    {
        throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected '('");
    }
    if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*currIt); sepTok == nullptr || sepTok->getSeparator() != Separator::BracketOpen)
    {
        throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected '('");
    }
    currIt++;
    if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*currIt); sepTok != nullptr && sepTok->getSeparator() != Separator::BracketClose)
    {
        end = currIt;
        m_currentFunction = new FunctionTokenSequence(func);
        m_funcs.push_back(m_currentFunction);
        // push token onto the stack?
        return;
    }
    while (currIt != m_parser.getTokens().end())
    {
        FunctionArgInfo arg;
        if (IdToken *funcNameTok = dynamic_cast<IdToken *>(*currIt); funcNameTok != nullptr)
        {
            arg.nameId = funcNameTok->getId();
            func->arguments.push_back(arg);
        }
        else
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected argument name");
        }
        currIt++;
        if (currIt == m_parser.getTokens().end())
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Missing ')'");
        }
        if (SeparatorToken *argSepTok = dynamic_cast<SeparatorToken *>(*currIt); argSepTok != nullptr)
        {
            if (argSepTok->getSeparator() == Separator::BracketClose)
            {
                end = currIt;
                m_currentFunction = new FunctionTokenSequence(func);
                m_funcs.push_back(m_currentFunction);
                return;
            }
        }
        else
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected ',' or ')'");
        }
        currIt++;
    }
}

bool GobLang::Compiler::ReversePolishGenerator::_isElseChainToken(std::vector<Token *>::const_iterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return false;
    }
    KeywordToken *key = dynamic_cast<KeywordToken *>(*it);
    return key != nullptr && key->getKeyword() == Keyword::Else;
}

bool GobLang::Compiler::ReversePolishGenerator::_isElifChainToken(std::vector<Token *>::const_iterator const &it)
{
    if (it == m_parser.getTokens().end())
    {
        return false;
    }
    KeywordToken *key = dynamic_cast<KeywordToken *>(*it);
    return key != nullptr && key->getKeyword() == Keyword::Elif;
}

void GobLang::Compiler::ReversePolishGenerator::_printTokenStack()
{
    std::cout << "STack: " << std::endl;
    for (std::vector<Token *>::iterator it = m_stack.begin(); it != m_stack.end(); it++)
    {
        std::cout << (*it)->toString() << std::endl;
    }
}

bool GobLang::Compiler::ReversePolishGenerator::_isBranchKeyword(std::vector<Token *>::const_iterator const &it)
{
    if (KeywordToken *keyTok = dynamic_cast<KeywordToken *>(*it); keyTok != nullptr)
    {
        return keyTok->getKeyword() == Keyword::If || keyTok->getKeyword() == Keyword::Elif || keyTok->getKeyword() == Keyword::While;
    }
    return false;
}

void GobLang::Compiler::ReversePolishGenerator::_addOperator(std::vector<Token *>::const_iterator const &it)
{
    OperatorToken *tok = dynamic_cast<OperatorToken *>(*it);
    tok->setIsUnary(!_isValidBinaryOperation(it));
    m_stack.push_back(*it);
}

bool GobLang::Compiler::ReversePolishGenerator::_isValidBinaryOperation(std::vector<Token *>::const_iterator const &it)
{
    // this means that it is the first object in the code so there can be no previous objects to rely on
    // but that also means there could be no other operand here, so it *has* to be unary
    if (it == m_parser.getTokens().begin())
    {
        return false;
    }
    if (OperatorToken *sep = dynamic_cast<OperatorToken *>(*it); sep != nullptr && sep->getOperator() == Operator::Assign)
    {
        return true;
    }
    std::vector<Token *>::const_iterator prevIt = it - 1;
    if (SeparatorToken *sep = dynamic_cast<SeparatorToken *>(*prevIt); sep != nullptr)
    {
        return sep->getSeparator() == Separator::ArrayClose || sep->getSeparator() == Separator::BracketClose;
    }

    return dynamic_cast<IdToken *>(*prevIt) ||
           dynamic_cast<IntToken *>(*prevIt) ||
           dynamic_cast<FloatToken *>(*prevIt) ||
           dynamic_cast<CharToken *>(*prevIt) ||
           dynamic_cast<StringToken *>(*prevIt) ||
           dynamic_cast<BoolConstToken *>(*prevIt);
}
