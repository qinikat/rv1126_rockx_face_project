#ifndef _RKMEDIA_FFMPEG_CONFIG_H                                               
#define _RKMEDIA_FFMPEG_CONFIG_H  
#include <pthread.h>

#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include "libavutil/time.h"
}


#define NETWORK_NUM (70)
#define NETWORK_ADDR_LENGTH 5000

#define WIDTH 1920
#define HEIGHT 1080
#define GOPSIZE 25

#define FLV_PROTOCOL 0
#define TS_PROTOCOL 1

typedef struct
{
    AVStream *stream; 
    AVCodecContext *enc;
    int64_t next_timestamp;
    int samples_count;
    AVPacket *packet;
} OutputStream;

typedef struct
{
    int width;
    int height;
    unsigned int config_id;
    int protocol_type; //流媒体TYPE
    char network_addr[NETWORK_ADDR_LENGTH];//流媒体地址
    enum AVCodecID video_codec; //视频编码器ID
    enum AVCodecID audio_codec; //音频编码器ID
    OutputStream video_stream; //VIDEO的STREAM配置
    OutputStream audio_stream; //AUDIO的STREAM配置
    AVFormatContext *oc; //是存储音视频封装格式中包含的信息的结构体，也是FFmpeg中统领全局的结构体，对文件的封装、编码操作从这里开始。

} RKMEDIA_FFMPEG_CONFIG; //FFMPEG配置

int init_rkmedia_ffmpeg_function();                                  
int set_rkmedia_ffmpeg_config(unsigned int config_id, RKMEDIA_FFMPEG_CONFIG *ffmpeg_config);

unsigned int get_rkmedia_ffmpeg_config(unsigned int config_id, RKMEDIA_FFMPEG_CONFIG *ffmpeg_config);
int init_rkmedia_ffmpeg_context(RKMEDIA_FFMPEG_CONFIG *ffmpeg_config);
void free_stream(AVFormatContext *oc, OutputStream *ost);

#endif