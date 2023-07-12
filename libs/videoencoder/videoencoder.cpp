#include "videoencoder.hpp"

VideoEncoder::VideoEncoder()
{
    int ret = -1;
    encoder_ = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_H264));
    encPkt_ = av_packet_alloc();

    encCtx_ = avcodec_alloc_context3(const_cast<const AVCodec*>(encoder_));
    encCtx_->codec_id = AV_CODEC_ID_H264;
    encCtx_->bit_rate = 1600000; //  4000K for 1080P, 3200K for 720P
    encCtx_->width = 1920;
    encCtx_->height = 1080;
    encCtx_->time_base = (AVRational){1,24};
    encCtx_->framerate = (AVRational){24,1};
    encCtx_->gop_size = 12;
    encCtx_->pix_fmt = AV_PIX_FMT_YUV444P;
    encCtx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "profile", "high444", 0);
    av_dict_set(&opts, "preset", "superfast", 0);
    av_dict_set(&opts, "tune", "zerolatency", 0);
    ret = avcodec_open2(encCtx_, encoder_, &opts);
    if(ret < 0){
        fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
        exit(1);
    }
    av_dict_free(&opts);
}

VideoEncoder::~VideoEncoder()
{
    av_packet_unref(encPkt_);
    av_packet_free(&encPkt_);
    avcodec_close(encCtx_);
    avcodec_free_context(&encCtx_);
}

bool VideoEncoder::EncodeFrame(AVFrame *inFrame)
{
    int ret = -1;

    av_packet_unref(encPkt_);
    pktCount_ = inFrame->pts;
    ret = avcodec_send_frame(encCtx_, inFrame);
    ret = avcodec_receive_packet(encCtx_, encPkt_);
    if(ret != 0) return 0;
    
    // printf("H%lld\n",pktCount_);
    encPkt_->pts = inFrame->pts;
    encPkt_->dts = inFrame->pts;
    encPkt_->duration = 1; /* #rgb frames in h264 pkt */

    return 1;
}

void VideoEncoder::Flush()
{
    avcodec_flush_buffers(encCtx_);
}
