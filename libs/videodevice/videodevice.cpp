#include "videodevice.hpp"


VideoDevice::VideoDevice(int deviceIndex)
{
    int ret = -1;
    avdevice_register_all();

    /* init informatContext */
    char vdev[2];
    sprintf(vdev, "%d", deviceIndex);
    AVDictionary *iopt = NULL;
    ret = av_dict_set(&iopt, "video_device_index", vdev, 0);
    ret = av_dict_set(&iopt, "framerate", "24", 0);
    ret = av_dict_set(&iopt, "pixel_format", "0rgb", 0);
    ret = av_dict_set(&iopt, "video_size", "1920x1080", 0);
    ret = av_dict_set(&iopt, "probesize", "100000000", 0);
    ret = av_dict_set(&iopt, "analyzeduration", "100000000", 0);
    ret = avformat_open_input(&deviceFmtCtx_, NULL , av_find_input_format("avfoundation"), &iopt);
    if(ret < 0){
        printf("Failed to open input format: %s\n",av_err2str(ret));
        exit(1);
    }
    av_dict_free(&iopt);
    
    for (int i = 0; i < deviceFmtCtx_->nb_streams; i++) {
        if(deviceFmtCtx_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
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

    decoder_ = const_cast<AVCodec*>(avcodec_find_decoder(deviceFmtCtx_->streams[streamIndex_]->codecpar->codec_id));
    decCtx_ = avcodec_alloc_context3(decoder_);
    ret = avcodec_parameters_to_context(decCtx_, deviceFmtCtx_->streams[streamIndex_]->codecpar);
    if(ret != 0 ){
        printf("Failed to copy parameters to decoder: %s\n",av_err2str(ret));
        exit(1);
    }
    ret = avcodec_open2(decCtx_, decoder_,NULL);
    if(ret != 0 ){
        printf("Failed to open decoder context: %s\n",av_err2str(ret));
        exit(1);
    }
    
    outFrame_ = av_frame_alloc();
    outFrame_->width = 1920;
    outFrame_->height = 1080;
    outFrame_->format = AV_PIX_FMT_0RGB;
    outFrame_->time_base = (AVRational){1,24};
    av_frame_get_buffer(outFrame_,0);

    startPTS_ = deviceFmtCtx_->streams[streamIndex_]->start_time;

    decCtx_->time_base = deviceFmtCtx_->streams[0]->time_base;
    decCtx_->sample_aspect_ratio = deviceFmtCtx_->streams[0]->sample_aspect_ratio;
    decCtx_->pix_fmt = (AVPixelFormat)deviceFmtCtx_->streams[0]->codecpar->format;

    rawPkt_ = av_packet_alloc();

}

VideoDevice::~VideoDevice()
{
    av_frame_unref(outFrame_);
    av_packet_unref(rawPkt_);
    avformat_free_context(deviceFmtCtx_);
    avcodec_close(decCtx_);
    av_frame_free(&outFrame_);
    av_packet_free(&rawPkt_);
    avformat_close_input(&deviceFmtCtx_);
    avcodec_free_context(&decCtx_);
}

void VideoDevice::ReadDevice()
{
    int ret = -1;

    while ((ret = av_read_frame(deviceFmtCtx_, rawPkt_)) != 0){}
    deltaT_ = rawPkt_->pts - startPTS_;
    frameCount_ = av_rescale_q(deltaT_, (AVRational){1,1000000}, (AVRational){1,24});
    fprintf(stdout,"V%lld\n",frameCount_);
    ret = avcodec_send_packet(decCtx_, rawPkt_);
    if(ret != 0)
    {
        fprintf(stderr, "Failed to send video packet: %s\n",av_err2str(ret));
        exit(1);
    }
    ret = avcodec_receive_frame(decCtx_, outFrame_);
    if(ret != 0)
    {
        fprintf(stderr, "Failed to receive video frame: %s\n",av_err2str(ret));
        exit(1);
    }
    outFrame_->pts = frameCount_;
    outFrame_->pkt_dts = frameCount_;

}

void VideoDevice::Flush()
{
    av_packet_unref(rawPkt_);
    avcodec_flush_buffers(decCtx_);
}