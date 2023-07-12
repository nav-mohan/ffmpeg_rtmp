#if !defined(FILEREADER_HPP)
#define FILEREADER_HPP

#include <fstream>
#include <string>

struct FileReader
{
    FileReader(const char *filename);
    std::string filename_;
    std::string content_;
    std::ifstream file_;
    ~FileReader();
    bool Refresh(); // 1 if new content, 0 otherwise
};

#endif // FILEREADER_HPP
