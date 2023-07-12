#if !defined(AUDIORESAMPLER_HPP)
#define AUDIORESAMPLER_HPP

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
}

struct AudioResampler
{
    AudioResampler(AVChannelLayout inChLayout, int inSampleRate, enum AVSampleFormat inSampleFmt, AVChannelLayout outChLayout, int outSampleRate, enum AVSampleFormat outSampleFmt);
    ~AudioResampler();
    struct SwrContext *swrCtx_;
    AVFrame *outFrame_;
    void Resample(AVFrame *inFrame);
};
#endif // AUDIORESAMPLER_HPP
