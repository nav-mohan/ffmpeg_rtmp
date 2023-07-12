extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{

    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("%s pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           avcodec_get_name(fmt_ctx->streams[pkt->stream_index]->codecpar->codec_id),
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}