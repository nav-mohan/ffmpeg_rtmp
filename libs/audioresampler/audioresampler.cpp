#include "audioresampler.hpp"

AudioResampler::AudioResampler(AVChannelLayout inChLayout, int inSampleRate, enum AVSampleFormat inSampleFmt, AVChannelLayout outChLayout, int outSampleRate, enum AVSampleFormat outSampleFmt)
{
    int ret = -1;
    swrCtx_ = swr_alloc();
    av_opt_set_chlayout  (swrCtx_, "in_chlayout",       &inChLayout,    0);
    av_opt_set_chlayout  (swrCtx_, "out_chlayout",      &outChLayout,   0);
    av_opt_set_int       (swrCtx_, "in_sample_rate",    44100,   0);
    av_opt_set_int       (swrCtx_, "out_sample_rate",   44100,  0);
    av_opt_set_sample_fmt(swrCtx_, "in_sample_fmt",     inSampleFmt,    0);
    av_opt_set_sample_fmt(swrCtx_, "out_sample_fmt",    outSampleFmt,   0);
    ret = swr_init(swrCtx_);
    if(ret != 0)
    {
        fprintf(stderr, "Failed to init audio resampler: %s\n",av_err2str(ret));
        exit(1);
    }

    outFrame_ = av_frame_alloc();
    outFrame_->nb_samples = 1024;
    outFrame_->sample_rate = outSampleRate;
    outFrame_->format = outSampleFmt;
    outFrame_->time_base = (AVRational){1,outSampleRate};
    av_channel_layout_copy(&outFrame_->ch_layout, &outChLayout);
    av_frame_get_buffer(outFrame_,0);
}

void AudioResampler::Resample(AVFrame *inFrame)
{
    int ret = -1;
    ret = swr_convert(
        swrCtx_,
        outFrame_->data,
        outFrame_->nb_samples,
        (const uint8_t **)(inFrame->data),
        inFrame->nb_samples
    );
    if(ret != outFrame_->nb_samples)
    {
        fprintf(stderr, "Failed to resample enough samples %d: %s\n",ret,av_err2str(ret));
        exit(1);
    }
    outFrame_->pts = inFrame->pts;
    outFrame_->pkt_dts = inFrame->pkt_dts;
}

AudioResampler::~AudioResampler()
{
    av_frame_unref(outFrame_);
    av_frame_free(&outFrame_);
    swr_free(&swrCtx_);
}