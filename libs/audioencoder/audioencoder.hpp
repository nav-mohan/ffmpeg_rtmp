#if !defined(AUDIOENCODER_HPP)
#define AUDIOENCODER_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

struct AudioEncoder
{
    AVCodec *encoder_;
    AVPacket *outPkt_;
    AVCodecContext *encCtx_;
    AudioEncoder();
    ~AudioEncoder();
    bool EncodeFrame(AVFrame *inFrame);
    int64_t curPktCount_ = 0;
    int64_t lastPktCount_ = 0;
    void Flush();
};

#endif // AUDIOENCODER_HPP
