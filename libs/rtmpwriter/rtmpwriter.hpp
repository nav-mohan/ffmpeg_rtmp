#if !defined(RTMPWRITER_HPP)
#define RTMPWRITER_HPP

#define RTMP_ADDRESS "rtmp://167.99.183.228/streaming"
#define FORMAT "flv"

// #define RTMP_ADDRESS "tmp/index.m3u8"
// #define FORMAT "hls"

// #define RTMP_ADDRESS "rtmp.aac"
// #define FORMAT "aac"

// #define RTMP_ADDRESS "rtmp.mp4"
// #define FORMAT "mp4"

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}
#include <mutex>

#include "helpers.h"

struct RtmpWriter
{
    AVFormatContext *outFmtCtx_;
    AVStream *videoStream_;
    AVStream *audioStream_;
    std::mutex mut_;

    RtmpWriter(AVCodecContext *videoCodecContext, AVCodecContext *audioCodecContext);
    ~RtmpWriter();
    void SendVideoPacket(AVPacket *pkt);
    void SendAudioPacket(AVPacket *pkt);
    void WriteTrailer();
};

#endif // RTMPWRITER_HPP
