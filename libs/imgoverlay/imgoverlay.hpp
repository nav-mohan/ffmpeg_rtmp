#if !defined(IMGOVERLAY_HPP)
#define IMGOVERLAY_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersrc.h>
    #include <libavfilter/buffersink.h>
    #include <libswscale/swscale.h>
    #include <libavutil/opt.h>
}

struct ImgOverlay
{
    ImgOverlay(const char *filename, int width, int height);
    ~ImgOverlay();
    AVFormatContext *imgFmtCtx_;
    AVCodecContext  *imgDecCtx_;
    AVFrame         *imgFrame_;
    AVFrame         *rescaledImgFrame_;
    AVPacket        *imgPkt_;
    AVFilterGraph   *filterGraph_;
    AVFilterContext *bufferSrc0Ctx_; // video
    AVFilterContext *bufferSrc1Ctx_; // image
    AVFilterContext *overlayCtx_;
    AVFilterContext *opacityCtx_;
    AVFilterContext *bufferSinkCtx_;
    AVFrame         *outFrame_;
    struct SwsContext *swsCtx_ = nullptr;
    void Flush();
    int width_ = 0;
    int height_ = 0;
    int xpos_ = 0;
    int ypos_ = 0;
    float opacity_ = 1.0;
    void InitFilterGraph(AVCodecContext *videoDecCtx, int xpos, int ypos, float opacity = 1.0);
    void OverlayImage(AVFrame *inFrame);
};

#endif // IMGOVERLAY_HPP
