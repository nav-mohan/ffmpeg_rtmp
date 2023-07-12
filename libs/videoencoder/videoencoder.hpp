#if !defined(VIDEOENCODER_HPP)
#define VIDEOENCODER_HPP


extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

struct VideoEncoder
{
    VideoEncoder();
    ~VideoEncoder();
    AVCodec *encoder_;
    AVCodecContext *encCtx_;
    AVPacket *encPkt_;
    bool EncodeFrame(AVFrame *inFrame);
    int64_t pktCount_ = 0;
    void Flush();
};

#endif // VIDEOENCODER_HPP
