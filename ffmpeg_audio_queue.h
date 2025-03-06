#ifndef _FFMPEG_AUDIO_QUEUE_H                                               
#define _FFMPEG_AUDIO_QUEUE_H  

#include "rkmedia_ffmpeg_config.h"
#include <pthread.h>
#include <queue>

using namespace std;

#define MAX_AUDIO_BUFFER_SIZE (1024 * 1024 * 3)

typedef struct audio_data_packet_t
{
    unsigned char buffer[MAX_AUDIO_BUFFER_SIZE];
    int audio_frame_size;

}audio_data_packet;


class AUDIO_QUEUE
{
public:
    //声明一个队列
    std::queue<audio_data_packet *> audio_packet_queue;
    //线程锁
    pthread_mutex_t audioMutex;
    //条件变量
    pthread_cond_t audioCond;

public:
    AUDIO_QUEUE();

    ~AUDIO_QUEUE();

    //入队函数
    int putAudioPacketQueue(audio_data_packet * audio_packet);
    //出队函数
    audio_data_packet *getAudioPacketQueue();

    //Mat getMatQueue_TimeOut();

    //队列大小
    int getAudioPacketQueueSize();
};

#endif