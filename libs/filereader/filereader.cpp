#include "filereader.hpp"

FileReader::FileReader(const char *filename) : filename_(filename), file_(filename){}

bool FileReader::Refresh()
{
    file_.open(filename_);
    std::string tempContent;
    std::getline(file_,tempContent);
    file_.close();
    if(content_ == tempContent) return 0;
    else 
    {
        content_ = std::move(tempContent);
        return 1;
    }
}