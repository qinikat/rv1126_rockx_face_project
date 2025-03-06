#ifndef _FFMPEG_VIDEO_QUEUE_H                                               
#define _FFMPEG_VIDEO_QUEUE_H  

#include "rkmedia_ffmpeg_config.h"
#include <pthread.h>
#include <queue>

using namespace std;

#define MAX_VIDEO_BUFFER_SIZE (1024*1024 * 3)

typedef struct _video_data_packet_t
{
    unsigned char buffer[MAX_VIDEO_BUFFER_SIZE];
    int video_frame_size;
    int frame_flag;
    
}video_data_packet_t;


class VIDEO_QUEUE
{
public:
    //声明一个队列
    std::queue<video_data_packet_t *> video_packet_queue;
    //线程锁
    pthread_mutex_t videoMutex;
    //条件变量
    pthread_cond_t videoCond;

public:
    VIDEO_QUEUE();

    ~VIDEO_QUEUE();

//==========Mat Queueu ==================

    //入队函数
    int putVideoPacketQueue(video_data_packet_t * video_packet);
    //出队函数
    video_data_packet_t *getVideoPacketQueue();

    //Mat getMatQueue_TimeOut();

    //队列大小
    int getVideoQueueSize();

};


#endif