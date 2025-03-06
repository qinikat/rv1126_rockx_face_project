#include "rkmedia_ffmpeg_config.h"

RKMEDIA_FFMPEG_CONFIG rkmedia_ffmpeg_configs[NETWORK_NUM];
pthread_mutex_t rkmedia_ffmpeg_config_mutex;

int init_rkmedia_ffmpeg_function()
{
    pthread_mutex_init(&rkmedia_ffmpeg_config_mutex, NULL);
    memset(rkmedia_ffmpeg_configs, 0, sizeof(rkmedia_ffmpeg_configs));
    return 0;
}

int set_rkmedia_ffmpeg_config(unsigned int config_id, RKMEDIA_FFMPEG_CONFIG *ffmpeg_config)
{
    pthread_mutex_lock(&rkmedia_ffmpeg_config_mutex);
    rkmedia_ffmpeg_configs[config_id] = *ffmpeg_config;
    pthread_mutex_unlock(&rkmedia_ffmpeg_config_mutex);
    return 0;
}

unsigned int get_rkmedia_ffmpeg_config(unsigned int config_id, RKMEDIA_FFMPEG_CONFIG *ffmpeg_config)
{
    pthread_mutex_lock(&rkmedia_ffmpeg_config_mutex);
    *ffmpeg_config = rkmedia_ffmpeg_configs[config_id];
    pthread_mutex_unlock(&rkmedia_ffmpeg_config_mutex);
    return 0;
}


int add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id, int width, int height)
{
    AVCodecContext *c = NULL;

    //创建输出码流的AVStream, AVStream是存储每一个视频/音频流信息的结构体
    ost->stream = avformat_new_stream(oc, NULL);
    if (!ost->stream)
    {
        printf("Can't not avformat_new_stream\n");
        return 0;
    }
    else
    {
        printf("Success avformat_new_stream\n");
    }

    //通过codecid找到CODEC
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec))
    {
        printf("Can't not find any encoder");
        return 0;
    }
    else
    {
        printf("Success find encoder");
    }

    //nb_streams 输入视频的AVStream 个数 就是当前有几种Stream，比如视频流、音频流、字幕，这样就算三种了,
    // s->nb_streams - 1其实对应的应是AVStream 中的 index
    ost->stream->id = oc->nb_streams - 1;
    //通过CODEC分配编码器上下文
    c = avcodec_alloc_context3(*codec);
    if (!c)
    {
        printf("Can't not allocate context3\n");
        return 0;
    }
    else
    {
        printf("Success allocate context3");
    }

    ost->enc = c;

    switch ((*codec)->type)
    {
    case AVMEDIA_TYPE_AUDIO:

        c->sample_fmt = (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP; //FFMPEG采样格式
        c->bit_rate = 153600;  //FFMPEG音频码率
        c->sample_rate = 48000; //FFMPEG采样率
        c->channel_layout = AV_CH_LAYOUT_STEREO;//FFMPEG声道数2
        c->channels = av_get_channel_layout_nb_channels(c->channel_layout); //FFMPEG采样通道
        ost->stream->time_base = (AVRational){1, c->sample_rate};//FFMPEG音频时间基
        break;

    case AVMEDIA_TYPE_VIDEO:

        //c->codec_id = codec_id;
        c->bit_rate = width * height * 3; //FFMPEG视频码率
        //分辨率必须是2的倍数
        c->width = width; //FFMPEG视频宽度
        c->height = height;//FFMPEG视频高度

        ost->stream->r_frame_rate.den = 1; //FFMPEG帧率,分母
        ost->stream->r_frame_rate.num = 25;//FFMPEG帧率,分子
        ost->stream->time_base = (AVRational){1, 25};//Stream视频时间基，默认情况下等于帧率

        c->time_base = ost->stream->time_base; //编码器时间基
        c->gop_size = GOPSIZE; //GOPSIZE
        c->pix_fmt = AV_PIX_FMT_NV12;//图像格式

        break;

    default:
        break;
    }

    //在h264头部添加SPS，PPS
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
    {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return 0;
}

//使能video编码器
int open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c = ost->enc;

    //打开编码器
    avcodec_open2(c, codec, NULL);

    //分配video avpacket包
    ost->packet = av_packet_alloc();

    /* 将AVCodecContext参数复制AVCodecParameters复用器 */
    avcodec_parameters_from_context(ost->stream->codecpar, c);
    return 0;
}

//使能audio编码器
int open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    AVCodecContext *c = ost->enc;

    //打开编码器
    avcodec_open2(c, codec, NULL);

    //分配 audio avpacket包
    ost->packet = av_packet_alloc();

    /* 将AVCodecContext参数复制AVCodecParameters复用器 */
    avcodec_parameters_from_context(ost->stream->codecpar, c); 
    return 0;
}

void free_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_close(ost->enc); //就是需要关闭的编码器的AVCodecContext
    avcodec_free_context(&ost->enc); //释放AVCodecContext上下文

    av_buffer_unref(&(ost->packet->buf));//释放AVBufferRef

    av_packet_unref(ost->packet); //释放AVPacket
    av_packet_free(&ost->packet); //释放AVPacket
}


int init_rkmedia_ffmpeg_context(RKMEDIA_FFMPEG_CONFIG *ffmpeg_config)
{
    AVOutputFormat *fmt = NULL;
    AVCodec *audio_codec = NULL;
    AVCodec *video_codec = NULL;
    int ret = 0;

    //FLV_PROTOCOL is RTMP TCP
    if (ffmpeg_config->protocol_type == FLV_PROTOCOL)
    {
        //初始化一个FLV的AVFormatContext
        ret = avformat_alloc_output_context2(&ffmpeg_config->oc, NULL, "flv", ffmpeg_config->network_addr); 
        if (ret < 0)
        {
            return -1;
        }
    }
    //TS_PROTOCOL is SRT UDP RTSP
    else if (ffmpeg_config->protocol_type == TS_PROTOCOL)
    {
        //初始化一个TS的AVFormatContext
        ret = avformat_alloc_output_context2(&ffmpeg_config->oc, NULL, "mpegts", ffmpeg_config->network_addr);
        if (ret < 0)
        {
            return -1;
        }
    }
  
    fmt = ffmpeg_config->oc->oformat;
    /*指定编码器*/
    fmt->video_codec = ffmpeg_config->video_codec;
    fmt->audio_codec = ffmpeg_config->audio_codec;

    if (fmt->video_codec != AV_CODEC_ID_NONE)
    {
        ret = add_stream(&ffmpeg_config->video_stream, ffmpeg_config->oc, &video_codec, fmt->video_codec,ffmpeg_config->width,ffmpeg_config->height);
        if (ret < 0)
        {
            avcodec_free_context(&ffmpeg_config->video_stream.enc);
            free_stream(ffmpeg_config->oc, &ffmpeg_config->video_stream);
            avformat_free_context(ffmpeg_config->oc);
            return -1;
        }

        ret = open_video(ffmpeg_config->oc, video_codec, &ffmpeg_config->video_stream, NULL);
        if (ret < 0)
        {
            avformat_free_context(ffmpeg_config->oc);
        }
    }

#if 0
    if (fmt->audio_codec != AV_CODEC_ID_NONE)
    {
        ret = add_stream(&ffmpeg_config->audio_stream, ffmpeg_config->oc, &audio_codec, fmt->audio_codec);
        if (ret < 0)
        {
            avcodec_free_context(&ffmpeg_config->audio_stream.enc);
            free_stream(ffmpeg_config->oc, &ffmpeg_config->audio_stream);
            avformat_free_context(ffmpeg_config->oc);
            return -1;
        }

        ret = open_audio(ffmpeg_config->oc, audio_codec, &ffmpeg_config->audio_stream, NULL);
        if (ret < 0)
        {
            avformat_free_context(ffmpeg_config->oc);
        }
    }
#endif

    av_dump_format(ffmpeg_config->oc, 0, ffmpeg_config->network_addr, 1);

    if (!(fmt->flags & AVFMT_NOFILE))
    {
        //打开输出文件
        ret = avio_open(&ffmpeg_config->oc->pb, ffmpeg_config->network_addr, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            free_stream(ffmpeg_config->oc, &ffmpeg_config->video_stream);
            free_stream(ffmpeg_config->oc, &ffmpeg_config->audio_stream);
            avformat_free_context(ffmpeg_config->oc);
            return -1;
        }
    }

    avformat_write_header(ffmpeg_config->oc, NULL);
    return 0;
}



