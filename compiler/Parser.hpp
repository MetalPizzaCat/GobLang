#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <exception>

#include "Token.hpp"
#include "SpecialCharacter.hpp"

namespace GobLang::Compiler
{
    class Parser
    {
    public:
        /**
         * @brief Moves code pointer to the next non-whitespace character
         *
         */
        void skipWhitespace();

         /**
         * @brief Checks if there is a comment and if so advances to the next line
         * 
         * @return true Found a comment and moved to the next line
         * @return false Didn't find any comments
         */
        bool skipComments();

        void parse();

        /**
         * @brief Verify that the char sequence used by the keyword is present
         *
         * @param keyword
         * @return true Keyword has been found and processed
         * @return false No keywords found
         */
        bool tryKeyword(std::string const &keyword);

        /**
         * @brief Verify that the char sequence used by operator is present
         *
         * @param op Operator to check
         * @return true
         * @return false
         */
        bool tryOperator(OperatorData const &op);

        /**
         * @brief Attempt to parse all known keywords
         *
         * @return KeywordToken* Pointer to the token for the parsed keyword or nullptr if no keywords were found
         */
        KeywordToken *parseKeywords();

        /**
         * @brief Attempt to parse all known operators
         *
         * @return OperatorToken* Pointer to token for the parsed operator or nullptr if no tokens were found
         */
        OperatorToken *parseOperators();

        IdToken *parseId();

        IntToken *parseInt();

        SeparatorToken *parseSeparators();

        StringToken *parseString();

        BoolConstToken *parseBool();

        CharToken *parseChar();

        SpecialCharacter const *parseSpecialCharacter(std::string::iterator const &it);

        /**
         * @brief Get iterator pointing to the end of the current line
         *
         * @return std::string::iterator
         */
        std::string::iterator getEndOfTheLine() { return (*m_lineIt).end(); }

        /**
         * @brief Advance current `m_rowIt` value by a given amount wrapping to the next line if necessary
         *
         * @param offset The amount to advance by
         * @param stopAtEndOfTheLine If true `m_rowIt` will always be at position upon line switch, otherwise it will continue till end of offset
         */
        void advanceRowIterator(size_t offset, bool stopAtEndOfTheLine = false);

        /**
         * @brief Advances `m_lineIt` and resets `m_rowIt` to the start of the new line
         * 
         * @param offset The amount to advance by
         */
        void advanceLineIterator(size_t offset);

        /**
         * @brief Get number of the the current line(row) starting from 0
         *
         * @return size_t Number of the current line starting from 0
         */
        size_t getLineNumber() const;

        /**
         * @brief Get number of the current character in the current row(line). Uses m_rowIt for calculation
         *
         * @return size_t
         */
        size_t getColumnNumber() const;

        /**
         * @brief Print information about lexemes
         *
         */
        void printInfoTable();

        /**
         * @brief Prints the parsed sequence using the token data
         *
         */
        void printCode();

        explicit Parser(std::vector<std::string> const &code);

        explicit Parser(std::string const &code);

        std::vector<Token *> const &getTokens() const { return m_tokens; }
        std::vector<std::string> const &getIds() const { return m_ids; }
        std::vector<int32_t> const &getInts() const { return m_ints; }

        size_t getTotalLineCount() const { return m_code.size(); }
        size_t getFinalLineRowCount() const { return m_code.rbegin()->size(); }
        ~Parser();

    private:
        std::vector<Token *> m_tokens;
        std::vector<std::string> m_ids;
        std::vector<std::string> m_strings;
        std::vector<int32_t> m_ints;
        std::vector<std::string> m_code;
        /**
         * @brief Pointer to the current character in the row
         *
         */
        std::string::iterator m_rowIt;
        /**
         * @brief Pointer to the current row
         *
         */
        std::vector<std::string>::iterator m_lineIt;
    };

    class ParsingError : public std::exception
    {
    public:
        const char *what() const throw() override;
        ParsingError(size_t row, size_t column, std::string const &msg);

        size_t getRow() const { return m_row; }
        size_t getColumn() const { return m_column; }

    private:
        std::string m_full;
        size_t m_row;
        size_t m_column;
        std::string m_message;
    };
}