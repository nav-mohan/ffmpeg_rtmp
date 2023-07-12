#include "rtmpwriter.hpp"

RtmpWriter::~RtmpWriter(){
    printf("Closing RtmpWriter\n");
    // WriteTrailer();
    avio_close(outFmtCtx_->pb);
}

RtmpWriter::RtmpWriter(AVCodecContext *videoCodecContext, AVCodecContext *audioCodecContext)
{
    int ret = -1;
    ret = avformat_alloc_output_context2(&outFmtCtx_, NULL, FORMAT, RTMP_ADDRESS);
    if(ret < 0 ){
        printf("Failed to allocate flv context : %s\n",av_err2str(ret));
        exit(1);
    }
    outFmtCtx_->start_time = 0;
    outFmtCtx_->start_time_realtime = 0;

    if(videoCodecContext)
    {
        videoStream_ = avformat_new_stream(outFmtCtx_, NULL);
        videoStream_->id = outFmtCtx_->nb_streams - 1;
        ret = avcodec_parameters_from_context(videoStream_->codecpar, videoCodecContext);
        videoStream_->start_time = 0;
    }

    if(audioCodecContext)
    {
        audioStream_ = avformat_new_stream(outFmtCtx_, NULL);
        audioStream_->id = outFmtCtx_->nb_streams - 1;
        ret = avcodec_parameters_from_context(audioStream_->codecpar, audioCodecContext);
        audioStream_->start_time = 0;
    }

    
    avio_open(&outFmtCtx_->pb, RTMP_ADDRESS, AVIO_FLAG_WRITE);

    ret = avformat_write_header(outFmtCtx_, NULL);

    av_dump_format(outFmtCtx_, 0, RTMP_ADDRESS, 1);

    /** avformat sets time_base to 1/90000 for HLS and 1/1000 for FLV when you open the format */
    // videoStream_->time_base = (AVRational){1,90000}; // HLS
    // audioStream_->time_base = (AVRational){1,90000}; // HLS
    // videoStream_->time_base = (AVRational){1,1000}; // FLV
    // audioStream_->time_base = (AVRational){1,1000}; // FLV

}

void RtmpWriter::SendVideoPacket(AVPacket *pkt)
{
    int ret = -1;

    pkt->stream_index = videoStream_->index;
    av_packet_rescale_ts(pkt, (AVRational){1,24},videoStream_->time_base);
    log_packet(outFmtCtx_, pkt);

    std::lock_guard<std::mutex> lock(mut_);
    ret = av_interleaved_write_frame(outFmtCtx_, pkt);
    if(ret < 0){
        printf("Failed to write video packet %s\n",av_err2str(ret));
        exit(1);
    }
    av_packet_unref(pkt);
}
void RtmpWriter::SendAudioPacket(AVPacket *pkt)
{
    int ret = -1;

    pkt->stream_index = audioStream_->index;
    av_packet_rescale_ts(pkt, (AVRational){1,44100}, audioStream_->time_base);
 
    log_packet(outFmtCtx_, pkt);
    std::lock_guard<std::mutex> lock(mut_);
    ret = av_interleaved_write_frame(outFmtCtx_, pkt);
    if(ret < 0){
        printf("Failed to write audio packet %d:%s\n",ret,av_err2str(ret));
        exit(1);
    }
    av_packet_unref(pkt);
}

void RtmpWriter::WriteTrailer()
{
    av_write_trailer(outFmtCtx_);
}