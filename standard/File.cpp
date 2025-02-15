#include "File.hpp"

MachineFunctions::File::FileNode::FileNode(std::string const &path, bool read)
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

MachineFunctions::File::FileNode::~FileNode()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

void MachineFunctions::File::openFile(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *read = m->getStackTopAndPop();
    MemoryValue *path = m->getStackTopAndPop();
    if (path == nullptr)
    {
        throw RuntimeException("Missing file name for file open operation");
    }
    if (read == nullptr || read->type != Type::Bool)
    {
        throw RuntimeException("Missing value for file read mode. Requires true for read and false for write");
    }
    if (StringNode *str = dynamic_cast<StringNode *>(std::get<MemoryNode *>(path->value)); str != nullptr)
    {
        FileNode *f = new FileNode(str->getString(), std::get<bool>(read->value));
        m->addObject(f);
        m->pushToStack(MemoryValue{.type = Type::MemoryObj, .value = f});
    }
    else
    {
        throw RuntimeException("File open argument is not a string");
    }

    delete path;
}

void MachineFunctions::File::closeFile(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *file = m->getStackTopAndPop();
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        fileNode->close();
        delete file;
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::isFileOpen(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *file = m->getStackTopAndPop();
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        m->pushToStack(MemoryValue{.type = Type::Bool, .value = fileNode->isOpen()});
        delete file;
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::writeToFile(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *text = m->getStackTopAndPop();
    MemoryValue *file = m->getStackTopAndPop();
    if (file == nullptr)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (text == nullptr)
    {
        throw RuntimeException("Expected string argument");
    }
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        fileNode->writeToFile(valueToString(*text));
        delete file;
        delete text;
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::readLineFromFile(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *file = m->getStackTopAndPop();
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {

        std::string str;
        if (std::getline(fileNode->getFile(), str))
        {
            m->pushToStack(MemoryValue{.type = Type::MemoryObj, .value = m->createString(str)});
        }
        else
        {
            m->pushToStack(MemoryValue{.type = Type::Null, .value = 0});
        }

        delete file;
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::isFileEnded(GobLang::Machine *m)
{
    using namespace GobLang;
    MemoryValue *file = m->getStackTopAndPop();
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        m->pushToStack(MemoryValue{.type = Type::Bool, .value = fileNode->isEof()});
        delete file;
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}
