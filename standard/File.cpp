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
    std::unique_ptr<MemoryValue> read = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    std::unique_ptr<MemoryValue> path = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
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
        FileNode *f = new FileNode(str->getString(), std::get<bool>(read->value), m->getNativeStructure("File"));
        m->addObject(f);
        m->pushToStack(MemoryValue{.type = Type::MemoryObj, .value = f});
    }
    else
    {
        throw RuntimeException("File open argument is not a string");
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
    std::unique_ptr<MemoryValue> file = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        fileNode->close();
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeIsFileOpen(GobLang::Machine *m)
{
    std::unique_ptr<MemoryValue> file = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        m->pushToStack(MemoryValue{.type = Type::Bool, .value = fileNode->isOpen()});
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeWriteToFile(GobLang::Machine *m)
{
    using namespace GobLang;
    std::unique_ptr<MemoryValue> file = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    std::unique_ptr<MemoryValue> text = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    if (file == nullptr)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (text.get() == nullptr)
    {
        throw RuntimeException("Expected string argument");
    }
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        fileNode->writeToFile(valueToString(*text, false, 0));
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeReadLineFromFile(GobLang::Machine *m)
{
    using namespace GobLang;
    std::unique_ptr<MemoryValue> file = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
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
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}

void MachineFunctions::File::FileNode::nativeIsFileEnded(GobLang::Machine *m)
{
    std::unique_ptr<MemoryValue> file = std::unique_ptr<MemoryValue>(m->getStackTopAndPop());
    if (file->type != Type::MemoryObj)
    {
        throw RuntimeException("Expected file handle object");
    }
    if (FileNode *fileNode = dynamic_cast<FileNode *>(std::get<MemoryNode *>(file->value)))
    {
        m->pushToStack(MemoryValue{.type = Type::Bool, .value = fileNode->isEof()});
    }
    else
    {
        throw RuntimeException("Expected file handle object");
    }
}
