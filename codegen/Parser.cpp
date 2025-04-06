
#include <cctype>
#include <cstring>
#include <sstream>
#include <iostream>
#include <limits>
#include "Parser.hpp"

void GobLang::Codegen::Parser::skipWhitespace()
{
    while (std::isspace(*m_rowIt) && m_rowIt != getEndOfTheLine())
    {
        advanceRowIterator(1);
    }
    if (m_rowIt == getEndOfTheLine())
    {
        advanceLineIterator(1);
    }
}

bool GobLang::Codegen::Parser::skipComments()
{
    if ((m_rowIt != getEndOfTheLine() && *m_rowIt == '#') || m_rowIt == getEndOfTheLine())
    {
        advanceLineIterator(1);
        return true;
    }
    return false;
}

void GobLang::Codegen::Parser::parse()
{

    if (m_code.empty())
    {
        return;
    }
    std::vector<std::function<std::unique_ptr<Token> (void)>> parsers = {
        std::bind(&Parser::parseBool, this),
        std::bind(&Parser::parseNullConst, this),
        std::bind(&Parser::parseKeywords, this),
        std::bind(&Parser::parseOperators, this),
        std::bind(&Parser::parseFloat, this),
        std::bind(&Parser::parseHexInt, this),
        std::bind(&Parser::parseInt, this),
        std::bind(&Parser::parseHexUnsignedInt, this),
        std::bind(&Parser::parseUnsignedInt, this),
        std::bind(&Parser::parseChar, this),
        std::bind(&Parser::parseString, this),
        std::bind(&Parser::parseId, this),

        std::bind(&Parser::parseSeparators, this),
    };
    do
    {

        skipWhitespace();
        if (skipComments())
        {
            continue;
        }

        bool foundValidToken = false;
        for (std::function<std::unique_ptr<Token>  (void)> const &f : parsers)
        {
            std::unique_ptr<Token> token = f();
            if (token != nullptr)
            {
                m_tokens.push_back(std::move(token));
                foundValidToken = true;
                break;
            }
        }
        if (!foundValidToken)
        {
            throw ParsingError(getLineNumber(), getColumnNumber(), "Unknown character sequence");
        }
        skipWhitespace();
        skipComments();
    } while (m_rowIt != getEndOfTheLine() && m_lineIt != m_code.end());
}

bool GobLang::Codegen::Parser::tryKeyword(std::string const &keyword)
{
    for (size_t i = 0; i < keyword.size(); i++)
    {
        if ((m_rowIt + i) == getEndOfTheLine() || *(m_rowIt + i) != keyword[i])
        {
            return false;
        }
    }
    // there should be a non keyword character after the keyword for it to be valid
    if (m_rowIt + keyword.size() != getEndOfTheLine() && std::isalnum(*(m_rowIt + keyword.size())))
    {
        return false;
    }
    return true;
}

bool GobLang::Codegen::Parser::tryOperator(OperatorData const &op)
{
    for (size_t i = 0; i < strnlen(op.symbol, 3); i++)
    {
        if ((m_rowIt + i) == getEndOfTheLine() || *(m_rowIt + i) != op.symbol[i])
        {
            return false;
        }
    }
    return true;
}

std::unique_ptr<GobLang::Codegen::KeywordToken> GobLang::Codegen::Parser::parseKeywords()
{

    for (std::map<std::string, Keyword>::const_iterator it = Keywords.begin(); it != Keywords.end(); it++)
    {
        if (tryKeyword(it->first))
        {
            size_t row = getLineNumber();
            size_t column = getColumnNumber();
            advanceRowIterator(it->first.size());
            return std::make_unique<KeywordToken>( 
                row,
                column,
                it->second);
        }
    }
    return nullptr;
}

std::unique_ptr<GobLang::Codegen::OperatorToken> GobLang::Codegen::Parser::parseOperators()
{
    for (std::vector<OperatorData>::const_iterator it = Operators.begin(); it != Operators.end(); it++)
    {
        if (tryOperator(*it))
        {
            size_t row = getLineNumber();
            size_t column = getColumnNumber();
            size_t offset = strnlen(it->symbol, 3);
            advanceRowIterator(offset);
            return std::make_unique<OperatorToken>( row,
                                     column,
                                     &(*it));
        }
    }
    return nullptr;
}

std::unique_ptr<GobLang::Codegen::IdToken> GobLang::Codegen::Parser::parseId()
{
    // must start with a character or underscore
    if (!std::isalpha(*m_rowIt) && (*m_rowIt) != '_')
    {
        return nullptr;
    }
    std::string id;
    size_t offset = 0;
    while ((m_rowIt + offset) != getEndOfTheLine() && (std::isalnum(*(m_rowIt + offset)) || *(m_rowIt + offset) == '_'))
    {
        id.push_back(*(m_rowIt + offset));
        offset++;
    }
    std::vector<std::string>::iterator it = std::find(m_ids.begin(), m_ids.end(), id);
    size_t index = std::string::npos;
    if (it == m_ids.end())
    {
        m_ids.push_back(id);
        index = m_ids.size() - 1;
    }
    else
    {
        index = it - m_ids.begin();
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<IdToken>( row, column, index);
}

std::unique_ptr<GobLang::Codegen::IntToken> GobLang::Codegen::Parser::parseInt()
{
    if (!std::isdigit(*m_rowIt))
    {
        return nullptr;
    }
    std::string num;
    size_t offset = 0;
    while ((m_rowIt + offset) != getEndOfTheLine() && std::isdigit(*(m_rowIt + offset)))
    {
        num.push_back(*(m_rowIt + offset));
        offset++;
    }
    int32_t numVal;
    try
    {
        numVal = std::stoi(num);
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throw ParsingError(
            m_rowIt - m_lineIt->begin(),
            m_lineIt - m_code.begin(),
            "Constant number is too large, valid range is " +
                std::to_string(std::numeric_limits<int32_t>::min()) +
                "< x < " +
                std::to_string(std::numeric_limits<int32_t>::max()));
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<IntToken>( row, column, numVal);
}

std::unique_ptr<GobLang::Codegen::IntToken> GobLang::Codegen::Parser::parseHexInt()
{
    // hex numbers follow 0x1234abcd pattern
    if (m_rowIt + 2 == getEndOfTheLine() || !((*m_rowIt == '0' && *(m_rowIt + 1) == 'x')))
    {
        return nullptr;
    }

    size_t offset = 2;
    if (!isValidHexDigit(*(m_rowIt + offset)))
    {
        return nullptr;
    }
    std::string num;
    while ((m_rowIt + offset) != getEndOfTheLine() && isValidHexDigit(*(m_rowIt + offset)))
    {
        num.push_back(*(m_rowIt + offset));
        offset++;
    }
    int32_t numVal;
    try
    {
        numVal = std::stoi(num, nullptr, 16);
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throw ParsingError(
            m_rowIt - m_lineIt->begin(),
            m_lineIt - m_code.begin(),
            "Constant number is too large, valid range is " +
                std::to_string(std::numeric_limits<int32_t>::min()) +
                "< x < " +
                std::to_string(std::numeric_limits<int32_t>::max()));
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<IntToken>( row, column, numVal);
}

std::unique_ptr<GobLang::Codegen::UnsignedIntToken> GobLang::Codegen::Parser::parseUnsignedInt()
{
    if (*m_rowIt != 'u')
    {
        return nullptr;
    }
    std::string num;
    size_t offset = 1;
    while ((m_rowIt + offset) != getEndOfTheLine() && std::isdigit(*(m_rowIt + offset)))
    {
        num.push_back(*(m_rowIt + offset));
        offset++;
    }
    uint64_t numVal;
    try
    {
        // c++ doesn't have conversion from str to uint
        // weird
        // but just throw an error ourselves then
        numVal = std::stoul(num);
        if (numVal > (uint64_t)std::numeric_limits<uint32_t>::max() || numVal < (uint64_t)std::numeric_limits<uint32_t>::min())
        {
            throw std::out_of_range("Unsigned int constant out of range");
        }
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throw ParsingError(
            m_rowIt - m_lineIt->begin(),
            m_lineIt - m_code.begin(),
            "Constant number is too large, valid range is " +
                std::to_string(std::numeric_limits<uint32_t>::min()) +
                "< x < " +
                std::to_string(std::numeric_limits<uint32_t>::max()));
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<UnsignedIntToken>( row, column, (uint32_t)numVal);
}

std::unique_ptr<GobLang::Codegen::UnsignedIntToken> GobLang::Codegen::Parser::parseHexUnsignedInt()
{
    // unsigned hex numbers follow u0x1234abcd pattern
    if (m_rowIt + 3 == getEndOfTheLine() || !(*m_rowIt == 'u' && (*(m_rowIt + 1) == '0' && *(m_rowIt + 2) == 'x')))
    {
        return nullptr;
    }

    size_t offset = 3;
    if (!isValidHexDigit(*(m_rowIt + offset)))
    {
        return nullptr;
    }
    std::string num;
    while ((m_rowIt + offset) != getEndOfTheLine() && isValidHexDigit(*(m_rowIt + offset)))
    {
        num.push_back(*(m_rowIt + offset));
        offset++;
    }
    uint64_t numVal;
    try
    {
        numVal = std::stoul(num, nullptr, 16);
        if (numVal > (uint64_t)std::numeric_limits<uint32_t>::max() || numVal < (uint64_t)std::numeric_limits<uint32_t>::min())
        {
            throw std::out_of_range("Unsigned int constant out of range");
        }
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throw ParsingError(
            m_rowIt - m_lineIt->begin(),
            m_lineIt - m_code.begin(),
            "Constant number is too large, valid range is " +
                std::to_string(std::numeric_limits<uint32_t>::min()) +
                "< x < " +
                std::to_string(std::numeric_limits<uint32_t>::max()));
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<UnsignedIntToken>( row, column, numVal);
}

std::unique_ptr<GobLang::Codegen::FloatToken> GobLang::Codegen::Parser::parseFloat()
{
    if (!std::isdigit(*m_rowIt))
    {
        return nullptr;
    }
    std::string num;
    size_t offset = 0;
    bool hasSeparator = false;
    while ((m_rowIt + offset) != getEndOfTheLine() && (std::isdigit(*(m_rowIt + offset)) || (*(m_rowIt + offset)) == '.'))
    {
        if ((*(m_rowIt + offset)) == '.')
        {
            if (!hasSeparator)
            {
                hasSeparator = true;
            }
            else
            {
                throw ParsingError(m_rowIt - m_lineIt->begin(),
                                   m_lineIt - m_code.begin(), "Malformed float constant");
            }
        }
        num.push_back(*(m_rowIt + offset));
        offset++;
    }
    if (!hasSeparator)
    {
        return nullptr;
    }
    float numVal;
    try
    {
        numVal = std::stof(num);
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throw ParsingError(
            m_rowIt - m_lineIt->begin(),
            m_lineIt - m_code.begin(),
            "Constant number is too large, valid range is " +
                std::to_string(std::numeric_limits<float>::min()) +
                "< x < " +
                std::to_string(std::numeric_limits<float>::max()));
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset);
    return std::make_unique<FloatToken>( row, column, numVal);
}

std::unique_ptr<GobLang::Codegen::SeparatorToken> GobLang::Codegen::Parser::parseSeparators()
{

    for (std::vector<SeparatorData>::const_iterator it = Separators.begin(); it != Separators.end(); it++)
    {
        if (*m_rowIt == it->symbol)
        {
            size_t row = getLineNumber();
            size_t column = getColumnNumber();
            advanceRowIterator(1);
            return std::make_unique<SeparatorToken>( row, column, it->separator);
        }
    }
    return nullptr;
}

std::unique_ptr<GobLang::Codegen::StringToken> GobLang::Codegen::Parser::parseString()
{

    if (*m_rowIt != '"')
    {

        return nullptr;
    }
    // we don't need to store opening and closing marks
    size_t offset = 1;
    std::string str;
    for (; (m_rowIt + offset) != getEndOfTheLine() && *(m_rowIt + offset) != '"'; offset++)
    {
        if (SpecialCharacter const *spec = parseSpecialCharacter(m_rowIt + offset); spec != nullptr)
        {
            str.push_back(spec->character);
            offset++;
        }
        else
        {
            str.push_back(*(m_rowIt + offset));
        }
    }
    std::vector<std::string>::iterator it = std::find(m_ids.begin(), m_ids.end(), str);
    size_t index = std::string::npos;
    if (it == m_ids.end())
    {
        m_ids.push_back(str);
        index = m_ids.size() - 1;
    }
    else
    {
        index = it - m_ids.begin();
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset + 1);
    return std::make_unique<StringToken>( row, column, index);
}

GobLang::Codegen::SpecialCharacter const *GobLang::Codegen::Parser::parseSpecialCharacter(std::string::iterator const &it)
{
    std::vector<SpecialCharacter>::const_iterator charIt = std::find_if(
        SpecialCharacters.begin(),
        SpecialCharacters.end(),
        [it](SpecialCharacter const &ch)
        {
            for (size_t i = 0; i < strnlen(ch.sequence, 2); i++)
            {
                if (*(it + i) != ch.sequence[i])
                {
                    return false;
                }
            }
            return true;
        });
    return charIt == SpecialCharacters.end() ? nullptr : &*charIt;
}

std::unique_ptr<GobLang::Codegen::NullConstToken> GobLang::Codegen::Parser::parseNullConst()
{
    const char *nullKeyword = "null";
    const size_t nullSize = strlen("null");
    for (size_t i = 0; i < nullSize; i++)
    {
        if (*(m_rowIt + i) != nullKeyword[i])
        {
            return nullptr;
        }
    }
    // there should be a non keyword character after the keyword for it to be valid
    if (m_rowIt + nullSize != getEndOfTheLine() && std::isalnum(*(m_rowIt + nullSize)))
    {
        return nullptr;
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(nullSize);
    return std::make_unique<NullConstToken>( row, column);
}

std::unique_ptr<GobLang::Codegen::BoolConstToken> GobLang::Codegen::Parser::parseBool()
{
    std::map<std::string, bool>::const_iterator it = std::find_if(
        Booleans.begin(),
        Booleans.end(),
        [this](std::pair<std::string, bool> const &boo)
        {
            for (size_t i = 0; i < boo.first.size(); i++)
            {
                if (*(m_rowIt + i) != boo.first[i])
                {
                    return false;
                }
            }
            return true;
        });
    if (it == Booleans.end())
    {
        return nullptr;
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(it->first.size());
    return std::make_unique<BoolConstToken>( row, column, it->second);
}

std::unique_ptr<GobLang::Codegen::CharToken> GobLang::Codegen::Parser::parseChar()
{
    if (*m_rowIt != '\'')
    {
        return nullptr;
    }
    char ch = '\0';
    size_t offset = 1;
    if (SpecialCharacter const *spec = parseSpecialCharacter(m_rowIt + offset); spec != nullptr)
    {
        ch = spec->character;
        offset += strnlen(spec->sequence, 2);
    }
    else
    {
        ch = *(m_rowIt + offset);
        offset++;
    }
    if (*(m_rowIt + offset) != '\'')
    {
        char c = *(m_rowIt + offset);
        return nullptr;
    }
    size_t row = getLineNumber();
    size_t column = getColumnNumber();
    advanceRowIterator(offset + 1);

    return std::make_unique<CharToken>( row, column, ch);
}

void GobLang::Codegen::Parser::advanceRowIterator(size_t offset, bool stopAtEndOfTheLine)
{
    size_t currentOffset = 0;
    for (; currentOffset < offset && m_rowIt != getEndOfTheLine(); currentOffset++)
    {
        m_rowIt++;
    }
    if (m_rowIt == getEndOfTheLine())
    {
        m_lineIt++;
        if (m_lineIt == m_code.end())
        {
            return;
        }
        m_rowIt = m_lineIt->begin();
        if (currentOffset < offset && !stopAtEndOfTheLine)
        {
            advanceRowIterator(offset - currentOffset, stopAtEndOfTheLine);
        }
    }
}

void GobLang::Codegen::Parser::advanceLineIterator(size_t offset)
{
    m_lineIt += offset;
    if (m_lineIt != m_code.end())
    {
        m_rowIt = m_lineIt->begin();
    }
}

size_t GobLang::Codegen::Parser::getLineNumber() const
{
    return m_lineIt - m_code.begin();
}

size_t GobLang::Codegen::Parser::getColumnNumber() const
{
    return m_rowIt - m_lineIt->begin();
}

void GobLang::Codegen::Parser::printInfoTable()
{
    for (size_t i = 0; i < m_ids.size(); i++)
    {
        std::cout << "W" << i << ": " << m_ids[i] << std::endl;
    }
}

void GobLang::Codegen::Parser::printCode()
{
    for(auto const& it : m_tokens)
    {
        std::cout << it->toString() << " ";
    }
    std::cout << std::endl;
}
GobLang::Codegen::Parser::Parser(std::vector<std::string> const &code) : m_code(code)
{
    if (!code.empty())
    {
        m_lineIt = m_code.begin();
        m_rowIt = m_lineIt->begin();
    }
}

GobLang::Codegen::Parser::Parser(std::string const &code)
{
    std::stringstream stream(code);
    std::string to;
    while (std::getline(stream, to, '\n'))
    {
        m_code.push_back(to);
    }
    m_lineIt = m_code.begin();
    m_rowIt = m_lineIt->begin();
}

GobLang::Codegen::Parser::~Parser()
{
    m_tokens.clear();
}

const char *GobLang::Codegen::ParsingError::what() const throw()
{
    return m_full.c_str();
}

GobLang::Codegen::ParsingError::ParsingError(size_t row, size_t column, std::string const &msg) : m_row(row), m_column(column), m_message(msg)
{
    m_full = ("Error at line " + std::to_string(m_row + 1) + " row " + std::to_string(m_column + 1) + ": " + m_message);
}
