#if !defined(LISTDEVICE_HPP)
#define LISTDEVICE_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavdevice/avdevice.h>
}

#include <string>
#include <map>
#include <regex>
#include <sstream>
#include <streambuf>
#include <iostream>

struct ListDevice
{
    ListDevice();
    AVFormatContext *pFormatCtx_;
    std::map<int,std::string> vDevices_;
    std::map<int,std::string> aDevices_;
    void RefreshDevices();
    void PrintDevices();
    std::regex deviceNameRegex_;
    void QueryForDevices();
    int vdi_ = -1;
    int adi_ = -1;
};


#endif // LISTDEVICE_HPP
