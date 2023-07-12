#include "imgoverlay.hpp"

ImgOverlay::ImgOverlay(const char *filename, int width, int height)
{
    imgFmtCtx_ = avformat_alloc_context();
    filterGraph_ = avfilter_graph_alloc();
    imgPkt_ = av_packet_alloc();
    imgFrame_ = av_frame_alloc();
    outFrame_ = av_frame_alloc();

    int ret = -1;
    ret = avformat_open_input(&imgFmtCtx_, filename, nullptr, nullptr);
    ret = avformat_find_stream_info(imgFmtCtx_, nullptr);
    for(int i = 0; i < imgFmtCtx_->nb_streams; i++)
    {
        if(imgFmtCtx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            const AVCodec *decoder = avcodec_find_decoder(imgFmtCtx_->streams[i]->codecpar->codec_id);
            imgDecCtx_ = avcodec_alloc_context3(decoder);
            ret = avcodec_parameters_to_context(imgDecCtx_, imgFmtCtx_->streams[i]->codecpar);
            imgDecCtx_->framerate = av_guess_frame_rate(imgFmtCtx_, imgFmtCtx_->streams[i],nullptr);
            imgDecCtx_->time_base = av_inv_q(imgDecCtx_->framerate);
            ret = avcodec_open2(imgDecCtx_, decoder, nullptr);
            break;
        }
    }

    width_ = width;
    height_ = height;

    rescaledImgFrame_ = av_frame_alloc();
    rescaledImgFrame_->width = width_;
    rescaledImgFrame_->height = height_;
    rescaledImgFrame_->format = imgDecCtx_->pix_fmt;
    ret = av_frame_get_buffer(rescaledImgFrame_,0);
    swsCtx_ = sws_getContext(
        imgDecCtx_->width, imgDecCtx_->height, imgDecCtx_->pix_fmt,
        rescaledImgFrame_->width, rescaledImgFrame_->height, imgDecCtx_->pix_fmt,
        SWS_BILINEAR, nullptr,nullptr,nullptr
    );
    printf("%d,%d",rescaledImgFrame_->format,imgDecCtx_->pix_fmt);
    getchar();
}

void ImgOverlay::InitFilterGraph(AVCodecContext *videoDecCtx, int xpos, int ypos, float opacity)
{
    int ret = -1;
    char args[512];

    snprintf(args, sizeof(args), 
    "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        videoDecCtx->width, videoDecCtx->height, 
        videoDecCtx->pix_fmt,
        videoDecCtx->time_base.num, videoDecCtx->time_base.den, 
        videoDecCtx->sample_aspect_ratio.num, videoDecCtx->sample_aspect_ratio.den 
    );
    ret = avfilter_graph_create_filter(&bufferSrc0Ctx_, avfilter_get_by_name("buffer"), "in0", args, NULL, filterGraph_ );

    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUVA444P, AV_PIX_FMT_NONE };
    ret = avfilter_graph_create_filter(&bufferSinkCtx_, avfilter_get_by_name("buffersink"), "out", NULL, NULL, filterGraph_);
    ret = av_opt_set_int_list(bufferSinkCtx_, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

    snprintf(args, sizeof(args), 
    "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        width_,height_,
        // imgDecCtx_->width,imgDecCtx_->height,
        imgDecCtx_->pix_fmt,
        // imgDecCtx_->time_base.num,imgDecCtx_->time_base.den,
        videoDecCtx->time_base.num, videoDecCtx->time_base.den, // The two overlays need to have exact time-stamps. There might be a smarter way of rescaling the time_bases of the videoFrame and imgFrame but this works too.
        imgDecCtx_->sample_aspect_ratio.num, imgDecCtx_->sample_aspect_ratio.den
    );
    ret = avfilter_graph_create_filter(&bufferSrc1Ctx_, avfilter_get_by_name("buffer"), "in1", args, NULL, filterGraph_);

    snprintf(args, sizeof(args), "x=%d:y=%d",xpos,ypos);
    ret = avfilter_graph_create_filter(&overlayCtx_, avfilter_get_by_name("overlay"), "overlay", args, NULL, filterGraph_);

    snprintf(args, sizeof(args), "aa=%f",opacity);
    ret = avfilter_graph_create_filter(&opacityCtx_, avfilter_get_by_name("colorchannelmixer"), "opacity", args, NULL, filterGraph_);

    ret = avfilter_link(bufferSrc0Ctx_, 0, overlayCtx_,     0);
    ret = avfilter_link(bufferSrc1Ctx_, 0, opacityCtx_,     0);
    ret = avfilter_link(opacityCtx_,    0, overlayCtx_,     1);
    ret = avfilter_link(overlayCtx_,    0, bufferSinkCtx_,  0);
    
    ret = avfilter_graph_config(filterGraph_, NULL);
}

void ImgOverlay::OverlayImage(AVFrame *inFrame)
{
    int ret = -1;
    int pts = inFrame->pts;

    avio_seek(imgFmtCtx_->pb,0,0);
    ret = av_read_frame(imgFmtCtx_, imgPkt_);
    ret = avcodec_send_packet(imgDecCtx_, imgPkt_);
    ret = avcodec_receive_frame(imgDecCtx_, imgFrame_);

    imgFrame_->pts = pts;
    imgFrame_->pkt_dts = pts;

    ret = sws_scale_frame(swsCtx_,rescaledImgFrame_,imgFrame_);

    rescaledImgFrame_->pts = pts;
    rescaledImgFrame_->pkt_dts = pts;
    
    ret = av_buffersrc_add_frame_flags(bufferSrc0Ctx_, inFrame, AV_BUFFERSRC_FLAG_KEEP_REF);
    ret = av_buffersrc_add_frame_flags(bufferSrc1Ctx_, rescaledImgFrame_, AV_BUFFERSRC_FLAG_KEEP_REF);
    ret = av_buffersink_get_frame(bufferSinkCtx_, outFrame_);

    outFrame_->pts = pts;
    outFrame_->pkt_dts = pts;
}

void ImgOverlay::Flush()
{
    av_frame_unref(imgFrame_);
    av_frame_unref(outFrame_);
    av_packet_unref(imgPkt_);
    avcodec_flush_buffers(imgDecCtx_);
}