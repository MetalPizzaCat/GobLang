#pragma once
#include <fstream>
#include "../execution/Machine.hpp"

namespace MachineFunctions::File
{
    /**
     * @brief Very simple and rudimentary file access object. This provides either read or write access to a file.
     *
     * Primary purpose is to test out a different object binding system and is subject to change
     *
     */
    class FileNode : public GobLang::MemoryNode
    {
    public:
        explicit FileNode(std::string const &path, bool read);

        void writeToFile(std::string const &str);

        void close();

        bool isOpen() { return m_file.is_open(); }

        bool isEof() { return m_file.eof(); }

        std::string readLine();

        std::fstream &getFile() { return m_file; }

        virtual ~FileNode();

    private:
        std::fstream m_file;
    };

    /**
     * @brief Opens a file and pushes the pointer to the stack
     *
     * @param m
     */
    void openFile(GobLang::Machine *m);

    void closeFile(GobLang::Machine *m);

    void isFileOpen(GobLang::Machine *m);

    void writeToFile(GobLang::Machine *m);

    void readLineFromFile(GobLang::Machine *m);

    void isFileEnded(GobLang::Machine *m);
}