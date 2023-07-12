#include "audiodevice.hpp"

AudioDevice::AudioDevice(int deviceIndex)
{
    int ret = -1;
    avdevice_register_all();

    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;

    char adev[2];
    sprintf(adev, "%d", deviceIndex);
    AVDictionary *iopt = nullptr;
    ret = av_dict_set(&iopt, "audio_device_index", adev, 0);
    ret = avformat_open_input(&deviceFmtCtx_, NULL , av_find_input_format("avfoundation"), &iopt);
    if(ret != 0){
        printf("Failed to open input format: %s\n",av_err2str(ret));
        exit(1);
    }
    av_dict_free(&iopt);

    for (int i = 0; i < deviceFmtCtx_->nb_streams; i++) {
        if(deviceFmtCtx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            streamIndex_ = i;
            break;
    }
    if(streamIndex_ == -1) {
        printf("Failed to find streamIndex_ \n");
        exit(1);
    }
    
    ret = avformat_find_stream_info(deviceFmtCtx_, NULL);
    if(ret < 0){
        printf("Failed to find stream info :%s\n",av_err2str(ret));
        exit(1);
    }
    av_dump_format(deviceFmtCtx_, streamIndex_, NULL, 0);

    AVCodec* decoder = const_cast<AVCodec*>(avcodec_find_decoder(deviceFmtCtx_->streams[streamIndex_]->codecpar->codec_id));
    decCtx_ = avcodec_alloc_context3(decoder);
    ret = avcodec_parameters_to_context(decCtx_, deviceFmtCtx_->streams[streamIndex_]->codecpar);
    if(ret != 0){
        printf("Failed to copy Audio CodePArametrs to decCtx_ :%s\n",av_err2str(ret));
        exit(1);
    }
    ret = avcodec_open2(decCtx_, decoder, NULL);
    if(ret != 0){
        printf("Failed to open audio decCtx_ %s\n",av_err2str(ret));
        exit(1);
    }

    inFrame_ = av_frame_alloc();
    if(!inFrame_)
    {
        fprintf(stderr, "Failed to init audio inFrame_\n");
        exit(1);
    }
    inFrame_->nb_samples = 512;
    inFrame_->sample_rate = 44100;
    inFrame_->format = AV_SAMPLE_FMT_FLT;
    av_channel_layout_copy(&inFrame_->ch_layout, &stereo);
    ret = av_frame_get_buffer(inFrame_,0);
    if(ret != 0)
    {
        fprintf(stderr, "Failed to get inFrame_ buffer: %s\n",av_err2str(ret));
        exit(1);
    }

    doubleFrame_ = av_frame_alloc();
    if(!doubleFrame_)
    {
        fprintf(stderr, "Failed to init audio doubleFrame_\n");
        exit(1);
    }
    doubleFrame_->nb_samples = 1024;
    doubleFrame_->sample_rate = 44100;
    doubleFrame_->format = AV_SAMPLE_FMT_FLT;
    av_channel_layout_copy(&doubleFrame_->ch_layout, &stereo);
    ret = av_frame_get_buffer(doubleFrame_,0);
    if(ret != 0)
    {
        fprintf(stderr, "Failed to get doubleFrame_ buffer: %s\n",av_err2str(ret));
        exit(1);
    }

    startPTS_ = deviceFmtCtx_->streams[streamIndex_]->start_time;
    rawPkt_ = av_packet_alloc();
}

void AudioDevice::ReadDevice()
{
    int ret = -1;
    doubleFrame_->nb_samples = 0;
    for(int i = 0; i < 2; i++)
    {
        while((ret = av_read_frame(deviceFmtCtx_, rawPkt_))!=0){}
        deltaT_ = rawPkt_->pts - startPTS_;
        if(i == 0)
            sampleCount_ = av_rescale_q(deltaT_, (AVRational){1,1000000}, (AVRational){1,44100});
        fprintf(stdout, "A%lld\n",sampleCount_);
        
        ret = avcodec_send_packet(decCtx_, rawPkt_);
        if(ret != 0)
        {
            fprintf(stderr, "Failed to send audio packet: %s\n",av_err2str(ret));
            exit(1);
        }
        ret = avcodec_receive_frame(decCtx_, inFrame_);
        if(ret != 0)
        {
            fprintf(stderr, "Failed to receive audio frame: %s\n",av_err2str(ret));
            exit(1);
        }
        const uint8_t *srcData = inFrame_->data[0];
        uint8_t *dstData = doubleFrame_->data[0]+i*2*512*4;
        memcpy(dstData,srcData, 2*512*4);
        doubleFrame_->nb_samples += 512;
        Flush();
    }

    doubleFrame_->pts = sampleCount_ + bump_;
    doubleFrame_->pkt_dts = sampleCount_ + bump_;
}

void AudioDevice::Flush()
{
    av_frame_unref(inFrame_);
    av_packet_unref(rawPkt_);
    avcodec_flush_buffers(decCtx_);

}

void AudioDevice::BumpSampleCount(int count)
{
    bump_ += count;
}

AudioDevice::~AudioDevice()
{
    av_packet_unref(rawPkt_);
    av_frame_unref(inFrame_);
    av_frame_unref(doubleFrame_);
    avformat_free_context(deviceFmtCtx_);
    avformat_close_input(&deviceFmtCtx_);
    avcodec_close(decCtx_);
    avcodec_free_context(&decCtx_);
    av_frame_free(&doubleFrame_);
    av_frame_free(&inFrame_);
    av_packet_free(&rawPkt_);
}
