#include "audioencoder.hpp"

AudioEncoder::~AudioEncoder()
{
    av_packet_unref(outPkt_);
    av_packet_free(&outPkt_);
    avcodec_close(encCtx_);
    avcodec_free_context(&encCtx_);
}

AudioEncoder::AudioEncoder()
{
    encoder_ = const_cast<AVCodec*>(avcodec_find_encoder(AV_CODEC_ID_AAC));
    outPkt_ = av_packet_alloc();

    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    encCtx_ = avcodec_alloc_context3(const_cast<const AVCodec*>(encoder_));
    av_channel_layout_copy(&encCtx_->ch_layout, &stereo);
    encCtx_->codec_id = AV_CODEC_ID_AAC;
    encCtx_->bit_rate = 192000;
    encCtx_->time_base = (AVRational){1,44100};
    encCtx_->sample_rate = 44100;
    encCtx_->sample_fmt = AV_SAMPLE_FMT_FLTP;
    encCtx_->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    AVDictionary *opts = nullptr;
    avcodec_open2(encCtx_, encoder_, &opts);
}

bool AudioEncoder::EncodeFrame(AVFrame *inFrame)
{
    int ret = -1;
    av_packet_unref(outPkt_);
    curPktCount_ = inFrame->pts;
    ret = avcodec_send_frame(encCtx_, inFrame);
    ret = avcodec_receive_packet(encCtx_, outPkt_);
    if(ret != 0) {printf("*\n");return 0;}
    if(curPktCount_ < lastPktCount_) return 0;
    
    outPkt_->pts = curPktCount_;
    outPkt_->dts = curPktCount_;
    outPkt_->time_base = (AVRational){1,44100};
    outPkt_->duration = (curPktCount_ - lastPktCount_); /* #pcm samples in AAC pkt */

    lastPktCount_ = curPktCount_;

    return 1;
}

void AudioEncoder::Flush()
{
    avcodec_flush_buffers(encCtx_);
}