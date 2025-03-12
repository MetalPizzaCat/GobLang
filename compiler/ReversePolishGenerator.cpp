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
                addToken(local);
                m_compilerTokens.push_back(local);
                m_isVariableDeclaration = false;
            }
            else if ((m_it + 1) != m_parser.getTokens().end() && _isIdUsedForFunctionCall(id->getId()))
            {
                // if we found that this id is used by a function we skip it and let function assign the local call
                if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*(m_it + 1)); sepTok != nullptr && sepTok->getSeparator() == Separator::BracketOpen)
                {
                    continue;
                }
            }
            else if (int32_t varId = _getLocalVariableAccessId(id->getId()); varId != -1)
            {
                LocalVarToken *local = new LocalVarToken(id->getRow(), id->getColumn(), varId);
                addToken(local);
                m_compilerTokens.push_back(local);
            }
            else
            {
                addToken(*m_it);
            }
        }
        else if (dynamic_cast<IntToken *>(*m_it) ||
                 dynamic_cast<UnsignedIntToken *>(*m_it) ||
                 dynamic_cast<FloatToken *>(*m_it) ||
                 dynamic_cast<CharToken *>(*m_it) ||
                 dynamic_cast<StringToken *>(*m_it) ||
                 dynamic_cast<BoolConstToken *>(*m_it) ||
                 dynamic_cast<NullConstToken *>(*m_it))
        {
            addToken(*m_it);
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
                addToken(popStack());
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
        addToken(*it);
    }
    m_stack.clear();
}

void GobLang::Compiler::ReversePolishGenerator::dumpStackWhile(std::function<bool(Token *)> const &cond)
{
    for (int32_t i = m_stack.size() - 1; i >= 0 && cond(m_stack[i]); i--)
    {
        addToken(m_stack[i]);
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
            std::cout << (*tokIt)->toString() << " ";
        }
        std::cout << std::endl;
    }
}

void GobLang::Compiler::ReversePolishGenerator::printStructs()
{
    for (std::unique_ptr<Struct::Structure> const &str : m_structs)
    {
        std::cout << "Struct(" << str->name << "):" << std::endl;
        for (Field const &f : str->fields)
        {
            std::cout << f.name << std::endl;
        }
    }
    std::cout << "end_struct" << std::endl;
}

void GobLang::Compiler::ReversePolishGenerator::addToken(Token *token)
{
    if (m_currentFunction == nullptr)
    {
        m_code.push_back(token);
    }
    else
    {
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
        addToken(sepToken);
        break;
    case Separator::BracketOpen:
        if (IdToken *funcNameToken = dynamic_cast<IdToken *>(*(it - 1)); funcNameToken != nullptr)
        {
            _compileFunctionCall(funcNameToken, sepToken);
        }
        else if (!_isBranchKeyword(it - 1))
        {
            m_stack.push_back(*it);
        }

        break;
    case Separator::Comma:
        if (!m_multiArgSequences.empty())
        {
            dumpStackWhile([this](Token *t)
                           { return t != m_multiArgSequences.back(); });
            m_multiArgSequences.back()->increaseArgCount();
        }
        break;
    case Separator::ArrayOpen:
    {
        if (_isPreviousTokenValidArrayParent(it))
        {
            ArrayIndexToken *token = new ArrayIndexToken(sepToken->getRow(), sepToken->getColumn());
            m_compilerTokens.push_back(token);
            m_stack.push_back(token);
        }
        else
        {
            ArrayCreationToken *arrayCreation = new ArrayCreationToken(sepToken->getRow(), sepToken->getColumn());
            m_compilerTokens.push_back(arrayCreation);
            m_stack.push_back(arrayCreation);
            m_multiArgSequences.push_back(arrayCreation);
        }
    }
    break;
    case Separator::ArrayClose:

        if (m_stack.empty())
        {
            throw ParsingError(sepToken->getRow(), sepToken->getColumn(), "Encountered ']' without previous '['");
        }
        else
        {
            Token *arrayPopToken = nullptr;
            do
            {
                arrayPopToken = m_stack.back();
                m_stack.pop_back();
                if (ArrayCreationToken *arrayTok = dynamic_cast<ArrayCreationToken *>(arrayPopToken); arrayTok != nullptr)
                {
                    SeparatorToken *prev = dynamic_cast<SeparatorToken *>(*(it - 1));
                    if (!(prev != nullptr && (prev->getSeparator() == Separator::ArrayOpen || prev->getSeparator() == Separator::Comma)))
                    {
                        m_multiArgSequences.back()->increaseArgCount();
                    }
                    addToken(m_multiArgSequences.back());
                    m_multiArgSequences.pop_back();
                    break;
                }
                addToken(arrayPopToken);

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
                MultiArgToken *mArg = m_multiArgSequences.back();
                SeparatorToken *prev = dynamic_cast<SeparatorToken *>(*(it - 1));
                if (!(prev != nullptr && (prev->getSeparator() == Separator::BracketOpen || prev->getSeparator() == Separator::Comma)))
                {
                    mArg->increaseArgCount();
                }
                if (!mArg->validateArgumentCount())
                {
                    throw ParsingError(
                        m_multiArgSequences.back()->getRow(),
                        m_multiArgSequences.back()->getColumn(),
                        std::string("Invalid number of arguments. Expected ") + std::to_string(mArg->getExpectedArgumentCount()) + " Got " + std::to_string(mArg->getArgCount()));
                }
                addToken(mArg);
                m_multiArgSequences.pop_back();
                break;
            }
            else if (dynamic_cast<IfToken *>(t) != nullptr || dynamic_cast<WhileToken *>(t) != nullptr)
            {
                addToken(t);
                break;
            }
            else
            {
                addToken(t);
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
            addToken(new LocalVarShrinkToken(sepToken->getRow(), sepToken->getColumn(), m_blockVariables.rbegin()->size()));
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
                addToken(dest);
            }

            else if (WhileToken *whileToken = dynamic_cast<WhileToken *>(jump); whileToken != nullptr)
            {
                whileToken->setReturnMark(getMarkCounterAndAdvance());
                GotoToken *loopJump = new GotoToken(sepToken->getRow(), sepToken->getColumn(), whileToken->getReturnMark());
                m_compilerTokens.push_back(loopJump);
                addToken(loopJump);
            }
            jump->setMark(getMarkCounterAndAdvance());
            JumpDestinationToken *dest = new JumpDestinationToken(sepToken->getRow(), sepToken->getColumn(), jump->getMark());
            if (_isElseChainToken(it + 1) || _isElifChainToken(it + 1))
            {
                GotoToken *elseJump = new GotoToken(sepToken->getRow(), sepToken->getColumn());
                m_jumps.push_back(elseJump);
                m_compilerTokens.push_back(elseJump);
                addToken(elseJump);
            }

            m_compilerTokens.push_back(dest);
            addToken(dest);
        }
        else if (m_currentFunction != nullptr)
        {
            if (dynamic_cast<ReturnToken *>(*(it - 1)) == nullptr)
            {
                ReturnToken *ret = new ReturnToken((*it)->getRow(), (*it)->getColumn(), false);
                m_compilerTokens.push_back(ret);
                addToken(ret);
            }
            _popVariableBlock();
            m_currentFunction = nullptr;
        }
        break;

    default:
        throw ParsingError(sepToken->getRow(), sepToken->getColumn(), "Unexpected separator encountered");
    }
}

void GobLang::Compiler::ReversePolishGenerator::_compileFunctionCall(IdToken const *name, SeparatorToken const *sep)
{

    std::vector<std::unique_ptr<Structure>>::iterator constructorIt = std::find_if(m_structs.begin(), m_structs.end(),
                                                                                   [name, this](std::unique_ptr<Structure> const &s)
                                                                                   { return s->name == m_parser.getIds()[name->getId()]; });

    MultiArgToken *token;
    if (constructorIt != m_structs.end())
    {
        token = new ConstructorCallToken(sep->getRow(), sep->getColumn(), constructorIt - m_structs.begin(), 0);
    }
    else
    {
        std::vector<FunctionTokenSequence *>::iterator funcIt = std::find_if(m_funcs.begin(),
                                                                             m_funcs.end(),
                                                                             [name](FunctionTokenSequence *f)
                                                                             { return f->getInfo()->nameId == name->getId(); });

        if (funcIt != m_funcs.end())
        {
            token = new FunctionCallToken(sep->getRow(), sep->getColumn(), funcIt - m_funcs.begin(), (*funcIt)->getInfo()->arguments.size());
        }
        else
        {
            token = new FunctionCallToken(sep->getRow(), sep->getColumn());
        }
    }

    m_compilerTokens.push_back(token);
    m_stack.push_back(token);
    m_multiArgSequences.push_back(token);
}

bool GobLang::Compiler::ReversePolishGenerator::_isIdUsedForFunctionCall(size_t id)
{
    return std::find_if(m_funcs.begin(), m_funcs.end(), [id](FunctionTokenSequence *f)
                        { return f->getInfo()->nameId == id; }) != m_funcs.end() ||
           std::find_if(m_structs.begin(), m_structs.end(), [id, this](std::unique_ptr<Struct::Structure> const &s)
                        { return s->name == m_parser.getIds()[id]; }) != m_structs.end();
}

bool GobLang::Compiler::ReversePolishGenerator::_isPreviousTokenValidArrayParent(std::vector<Token *>::const_iterator const &it)
{
    // to avoid dealing with reverse iterators just check if it's the first one
    if (it == m_parser.getTokens().end())
    {
        return false;
    }

    if (SeparatorToken *sep = dynamic_cast<SeparatorToken *>(*(it - 1));
        sep != nullptr &&
        (sep->getSeparator() == Separator::ArrayClose || // function calls that return arrays
         sep->getSeparator() == Separator::BracketClose  // chained array access operators
         ))
    {
        return true;
    }
    return dynamic_cast<IdToken *>(*(it - 1)) != nullptr; // obvious ID[] usage
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
            addToken(contTok);
        }
        break;
    case Keyword::Break:
        if (WhileToken *whileTok = getCurrentLoop(); whileTok != nullptr)
        {
            LoopControlToken *contTok = new LoopControlToken(keyToken->getRow(), keyToken->getColumn(), true, whileTok);
            m_compilerTokens.push_back(contTok);
            addToken(contTok);
        }
        break;
    case Keyword::Function:
        // functions have their own way of being parsed to avoid messing with the header
        _compileFunction(it, m_it);
        if (m_currentFunction != nullptr)
        {
            _appendVariableBlock();
            for (
                std::vector<FunctionArgInfo>::const_iterator argIt = m_currentFunction->getInfo()->arguments.begin();
                argIt != m_currentFunction->getInfo()->arguments.end();
                argIt++)
            {
                _appendVariable(argIt->nameId);
            }
        }
        break;
    case Keyword::Return:
    {
        bool hasVal = true;
        if (it + 1 != m_parser.getTokens().end())
        {
            if (SeparatorToken *nextTok = dynamic_cast<SeparatorToken *>(*(it + 1)); nextTok != nullptr && nextTok->getSeparator() == Separator::End)
            {
                hasVal = false;
            }
        }
        if (m_currentFunction != nullptr)
        {
            ReturnToken *ret = new ReturnToken((*it)->getRow(), (*it)->getColumn(), hasVal);
            m_compilerTokens.push_back(ret);
            m_stack.push_back(ret);
        }
        else if (hasVal)
        {
            throw ParsingError(keyToken->getRow(), keyToken->getColumn(), "Return in main block can not have a value");
        }
        else
        {
            HaltToken *hlt = new HaltToken((*it)->getRow(), (*it)->getColumn());
            m_compilerTokens.push_back(hlt);
            m_stack.push_back(hlt);
        }
    }
    break;
    case Keyword::Struct:
        // benefit of jumping elsewhere for this is that we can be sure that struct inside a struct is not possible
        _compileStructure(it, m_it);
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
    if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*currIt); sepTok != nullptr && sepTok->getSeparator() == Separator::BracketClose)
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
    throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected ')'");
}

void GobLang::Compiler::ReversePolishGenerator::_compileStructure(
    std::vector<Token *>::const_iterator const &start,
    std::vector<Token *>::const_iterator &end)
{
    using namespace Struct;

    Structure *structure = new Structure();
    std::vector<Token *>::const_iterator currIt = start + 1;
    if (IdToken *funcNameTok = dynamic_cast<IdToken *>(*currIt); funcNameTok != nullptr)
    {
        // because structs are objects and might require access to its name
        // we store it as a string not id
        // TODO: Make sure that if string stripping gets added strings used as names don't get removed
        structure->name = m_parser.getIds()[funcNameTok->getId()];
    }
    else
    {
        throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Missing structure name");
    }
    currIt++;
    if (SeparatorToken *sepTok = dynamic_cast<SeparatorToken *>(*currIt); currIt == m_parser.getTokens().end() || sepTok == nullptr || sepTok->getSeparator() != Separator::BlockOpen)
    {
        throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected '{'");
    }
    currIt++;
    if (SeparatorToken *earlyEndTok = dynamic_cast<SeparatorToken *>(*currIt); earlyEndTok != nullptr && earlyEndTok->getSeparator() == Separator::BlockClose)
    {
        end = currIt;
        m_structs.push_back(std::unique_ptr<Structure>(structure));
        return;
    }

    while (currIt != m_parser.getTokens().end())
    {
        Field field;
        if (IdToken *fieldNameTok = dynamic_cast<IdToken *>(*currIt); fieldNameTok != nullptr)
        {
            field.name = m_parser.getIds()[fieldNameTok->getId()];
            structure->fields.push_back(field);
        }
        else
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected field name");
        }
        currIt++;
        if (currIt == m_parser.getTokens().end())
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Missing '}'");
        }
        if (SeparatorToken *argSepTok = dynamic_cast<SeparatorToken *>(*currIt); argSepTok != nullptr)
        {
            if (argSepTok->getSeparator() == Separator::BlockClose)
            {
                end = currIt;
                // store them as unique_ptr to make it easier to manage
                // but that does mean that it will be necessary to copy this data into the machine
                m_structs.push_back(std::unique_ptr<Structure>(structure));
                return;
            }
        }
        else
        {
            throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected ',' or '}'");
        }
        currIt++;
    }
    throw ParsingError((*start)->getRow(), (*start)->getColumn(), "Expected '}'");
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
           dynamic_cast<UnsignedIntToken *>(*prevIt) ||
           dynamic_cast<FloatToken *>(*prevIt) ||
           dynamic_cast<CharToken *>(*prevIt) ||
           dynamic_cast<StringToken *>(*prevIt) ||
           dynamic_cast<BoolConstToken *>(*prevIt) ||
           dynamic_cast<NullConstToken *>(*prevIt);
}
