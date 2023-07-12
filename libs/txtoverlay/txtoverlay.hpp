#if !defined(TXTOVERLAY_HPP)
#define TXTOVERLAY_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavfilter/avfilter.h>
    #include <libavfilter/buffersrc.h>
    #include <libavfilter/buffersink.h>
    #include <libavutil/opt.h>
}

#include <string>

struct TxtOverlay
{
    TxtOverlay();
    void InitFilterGraph(AVCodecContext *videoDecCtx, int fontsize, const char *fontcolor, int xpos, int ypos);
    void ReInitFilterGraph(); // call this after updating the content_
    std::string content_ = "Radio Western";
    ~TxtOverlay();
    AVFilterGraph *filterGraph_;
    AVFilterContext *bufferSrcCtx_;
    AVFilterContext *textCtx_;
    AVFilterContext *bufferSinkCtx_;
    AVFrame *outFrame_;
    void OverlayText(AVFrame *inFrame);
    void Flush();
};

#endif // TXTOVERLAY_HPP
