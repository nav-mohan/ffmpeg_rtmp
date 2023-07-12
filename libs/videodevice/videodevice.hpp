#if !defined(VIDEODEVICE_HPP)
#define VIDEODEVICE_HPP

extern "C"
{
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

struct VideoDevice
{
    AVFormatContext *deviceFmtCtx_ = nullptr;
    AVPacket *rawPkt_;
    AVCodec *decoder_;
    AVCodecContext *decCtx_;
    AVFrame *outFrame_;
    int streamIndex_ = -1;
    int64_t deltaT_ = 0;
    int64_t frameCount_ = 0;
    int64_t startPTS_ = 0;

    VideoDevice(int deviceIndex);
    ~VideoDevice();
    void ReadDevice();
    void Flush();
};

#endif // VIDEODEVICE_HPP
