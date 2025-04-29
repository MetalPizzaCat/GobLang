#include "File.hpp"
#include <memory>

using namespace GobLang;

MachineFunctions::File::FileNode::FileNode(std::string const &path, bool read, NativeStructureInfo const *info) : NativeStructureObjectNode(info)
{
    if (read)
    {
        m_file.open(path, std::fstream::in);
    }
    else
    {
        m_file.open(path, std::fstream::out);
    }
}

void MachineFunctions::File::FileNode::writeToFile(std::string const &str)
{
    m_file << str;
}

void MachineFunctions::File::FileNode::close()
{
    m_file.close();
}

std::string MachineFunctions::File::FileNode::readLine()
{
    std::string out;
    std::getline(m_file, out);
    return out;
}

void MachineFunctions::File::FileNode::constructor(GobLang::Machine *m)
{

    if (StringNode *path = m->popObjectFromStack<StringNode>(); path != nullptr)
    {

        FileNode *f = new FileNode(path->getString(), m->popFromStack<bool>(), m->getNativeStructure("File"));
        m->addObject(f);
        m->pushToStack(Value(f));
    }
    else
    {
        throw RuntimeException("Missing file name for file open operation");
    }
}

MachineFunctions::File::FileNode::~FileNode()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}
void MachineFunctions::File::FileNode::nativeCloseFile(GobLang::Machine *m)
{
    if (FileNode *file = m->popObjectFromStack<FileNode>(); file != nullptr)
    {
        file->close();
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeIsFileOpen(GobLang::Machine *m)
{
    if (FileNode *file = m->popObjectFromStack<FileNode>(); file != nullptr)
    {
        m->pushToStack(Value(file->isOpen()));
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeWriteToFile(GobLang::Machine *m)
{
    using namespace GobLang;
    if (FileNode *file = m->popObjectFromStack<FileNode>(); file != nullptr)
    {
        file->writeToFile(valueToString(m->getStackTopAndPop(), false, 0));
    }
    else
    {

        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeReadLineFromFile(GobLang::Machine *m)
{
    using namespace GobLang;
    if (FileNode *file = m->popObjectFromStack<FileNode>(); file != nullptr)
    {
        std::string str;
        if (std::getline(file->getFile(), str))
        {
            m->pushToStack(Value(m->createString(str)));
        }
        else
        {
            m->pushToStack(Value(nullptr));
        }
    }
    else
    {

        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeIsFileEnded(GobLang::Machine *m)
{
    using namespace GobLang;
    if (FileNode *file = m->popObjectFromStack<FileNode>(); file != nullptr)
    {
        m->pushToStack(Value(file->isEof()));
    }
    else
    {

        throw RuntimeException("Expected file handle object");
    }
}

std::string MachineFunctions::File::FileNode::toString(bool pretty, size_t depth)
{
    return "{NativeFileAccess}";
}
