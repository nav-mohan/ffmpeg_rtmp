#include "videorescaler.hpp"

VideoRescaler::VideoRescaler(int iWidth, int iHeight, enum AVPixelFormat iPixFmt, int oWidth, int oHeight, enum AVPixelFormat oPixFmt) : 
iWidth_(iWidth),iHeight_(iHeight),oWidth_(oWidth),oHeight_(oHeight),iPixFmt_(iPixFmt),oPixFmt_(oPixFmt)
{
    outFrame_ = av_frame_alloc();
    outFrame_->width = oWidth_;
    outFrame_->height = oHeight_;
    outFrame_->format = oPixFmt_;
    av_frame_get_buffer(outFrame_,0);

    swsCtx_ = sws_getContext(
        iWidth_, iHeight_, iPixFmt_,
        oWidth_, oHeight_, oPixFmt_,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    printf("INIT RESCALER %d,%d-->%d,%d\n",iWidth_,iHeight_,oWidth_,oHeight_);
}


void VideoRescaler::Rescale(AVFrame *inFrame)
{
    int ret = -1;
    printf("RESCALING %d,%d-->%d,%d\n",iWidth_,iHeight_,oWidth_,oHeight_);
    ret = sws_scale(
        swsCtx_,
        (const uint8_t * const *)inFrame, 
        inFrame->linesize, 
        0,
        inFrame->height,
        outFrame_->data,
        outFrame_->linesize
    );

    if(ret != outFrame_->height)
    {
        printf("Failed to rescale frame %d:%s\n",ret,av_err2str(ret));
        exit(1);
    }
    outFrame_->pts = inFrame->pts;
    outFrame_->pkt_dts = inFrame->pkt_dts;
}