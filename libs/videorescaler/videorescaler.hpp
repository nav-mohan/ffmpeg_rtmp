#if !defined(VIDEORESCALER_HPP)
#define VIDEORESCALER_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

struct VideoRescaler
{
    struct SwsContext *swsCtx_;
    AVFrame *outFrame_;
    void Rescale(AVFrame *inFrame);
    VideoRescaler(int iWidth, int iHeight, enum AVPixelFormat iPixFmt, int oWidth, int oHeight, enum AVPixelFormat oPixFmt);
    int iWidth_,iHeight_,oWidth_,oHeight_;
    enum AVPixelFormat iPixFmt_,oPixFmt_;
};

#endif // VIDEORESCALER_HPP