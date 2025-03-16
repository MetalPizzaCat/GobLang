#pragma once
#include <fstream>
#include "../execution/Machine.hpp"
#include "../execution/NativeStructure.hpp"

namespace MachineFunctions::File
{
    /**
     * @brief Very simple and rudimentary file access object. This provides either read or write access to a file.
     *
     * Primary purpose is to test out a different object binding system and is subject to change
     *
     */
    class FileNode : public GobLang::Struct::NativeStructureObjectNode
    {
    public:
        explicit FileNode(std::string const &path, bool read, NativeStructureInfo const* info);

        void writeToFile(std::string const &str);

        void close();

        bool isOpen() { return m_file.is_open(); }

        bool isEof() { return m_file.eof(); }

        std::string readLine();

        std::fstream &getFile() { return m_file; }

        /**
         * @brief Opens a file and pushes the pointer to the stack
         *
         * @param m
         */
        static void constructor(GobLang::Machine *m);

        static void nativeCloseFile(GobLang::Machine *m);

        static void nativeIsFileOpen(GobLang::Machine *m);

        static void nativeWriteToFile(GobLang::Machine *m);

        static void nativeReadLineFromFile(GobLang::Machine *m);

        static void nativeIsFileEnded(GobLang::Machine *m);

        virtual ~FileNode();

    private:
        std::fstream m_file;
    };
}