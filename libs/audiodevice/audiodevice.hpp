#if !defined(AUDIODEVICE_HPP)
#define AUDIODEVICE_HPP

extern "C"
{
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

struct AudioDevice
{
    AVFormatContext *deviceFmtCtx_  = nullptr;
    AVPacket *rawPkt_               = nullptr;
    AVCodecContext *decCtx_         = nullptr;
    AVFrame *inFrame_               = nullptr;
    AVFrame *doubleFrame_           = nullptr;
    int streamIndex_                = -1;
    int64_t deltaT_                 = 0;
    int64_t sampleCount_            = 0;
    int64_t bump_                   = 0;
    int64_t startPTS_               = 0;

    AudioDevice(int deviceIndex);
    ~AudioDevice();
    void ReadDevice();
    void BumpSampleCount(int count);
    void Flush();
};

#endif // AUDIODEVICE_HPP