#include "txtoverlay.hpp"

TxtOverlay::TxtOverlay(){}

void TxtOverlay::InitFilterGraph(AVCodecContext *videoDecCtx, int fontsize, const char *fontcolor, int xpos, int ypos)
{
    int ret = -1;
    char args[1024];

    filterGraph_ = avfilter_graph_alloc();
    
    snprintf(args, sizeof(args), 
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        videoDecCtx->width, videoDecCtx->height,
        videoDecCtx->pix_fmt,
        videoDecCtx->time_base.num, videoDecCtx->time_base.den,
        videoDecCtx->sample_aspect_ratio.num,videoDecCtx->sample_aspect_ratio.den
    );
    ret = avfilter_graph_create_filter(&bufferSrcCtx_, avfilter_get_by_name("buffer"), "in", args, NULL, filterGraph_);

    snprintf(args, sizeof(args), "text=%s:x=%d:y=%d:fontcolor=%s:fontsize=%d",content_.c_str(),xpos,ypos,fontcolor,fontsize);
    ret = avfilter_graph_create_filter(&textCtx_, avfilter_get_by_name("drawtext"), "text", args, NULL, filterGraph_);

    enum AVPixelFormat pix_fmts[] = { videoDecCtx->pix_fmt, AV_PIX_FMT_NONE };
    ret = avfilter_graph_create_filter(&bufferSinkCtx_, avfilter_get_by_name("buffersink"), "out", NULL, NULL, filterGraph_);
    ret = av_opt_set_int_list(bufferSinkCtx_, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

    ret = avfilter_link(bufferSrcCtx_, 0, textCtx_,     0);
    ret = avfilter_link(textCtx_, 0, bufferSinkCtx_,    0);
    
    ret = avfilter_graph_config(filterGraph_, NULL);

    outFrame_ = av_frame_alloc();
}

void TxtOverlay::OverlayText(AVFrame *videoFrame)
{
    int ret = -1;
    int pts = videoFrame->pts; // i need to store the pts before running it through the filter.

    ret = av_buffersrc_add_frame_flags(bufferSrcCtx_, videoFrame, AV_BUFFERSRC_FLAG_KEEP_REF);
    ret = av_buffersink_get_frame(bufferSinkCtx_, outFrame_);

    // restore original pts of videoFrame 
    outFrame_->pts =pts;
    outFrame_->pkt_dts =pts;
}

void TxtOverlay::ReInitFilterGraph()
{
    char args[1024];
    snprintf(args, sizeof(args), "text=%s",content_.c_str());
    avfilter_graph_queue_command(filterGraph_,"drawtext","reinit",args,0,0);
}

void TxtOverlay::Flush()
{
    av_frame_unref(outFrame_);
}