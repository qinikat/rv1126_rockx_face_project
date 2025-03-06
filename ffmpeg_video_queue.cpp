#include "ffmpeg_video_queue.h"

//VIDEO队列的构造器，包含mutex的初始化和条件变量初始化
VIDEO_QUEUE::VIDEO_QUEUE()
{
    pthread_mutex_init(&videoMutex, NULL);//mutex的初始化
    pthread_cond_init(&videoCond, NULL);//条件变量初始化
}

 //VIDEO队列的析构函数，锁的销毁和条件变量的销毁
VIDEO_QUEUE ::~VIDEO_QUEUE()
{
    pthread_mutex_destroy(&videoMutex);//锁的销毁
    pthread_cond_destroy(&videoCond);//条件变量的销毁
}


//VIDEO_QUEUE的插入视频队列操作
int VIDEO_QUEUE::putVideoPacketQueue(video_data_packet_t *video_packet)
{
    pthread_mutex_lock(&videoMutex); //上视频锁
    video_packet_queue.push(video_packet);//向视频队列插入video_data_packet_t包
    pthread_cond_broadcast(&videoCond);//唤醒视频队列
    pthread_mutex_unlock(&videoMutex);//解视频锁
    return 0;
}

//VIDEO_QUEUE取出视频包
video_data_packet_t *VIDEO_QUEUE::getVideoPacketQueue()
{
    pthread_mutex_lock(&videoMutex);//上视频锁
    while (video_packet_queue.size() == 0)
    {
        pthread_cond_wait(&videoCond, &videoMutex);  //当视频队列没有数据的时候，等待被唤醒
    }
    video_data_packet_t *item = video_packet_queue.front();//把视频数据包移到最前面
    video_packet_queue.pop();//pop取出视频数据并删除
    pthread_mutex_unlock(&videoMutex);//解视频锁
    return item;
}

//VIDEO_QUEUE视频队列长度
int VIDEO_QUEUE::getVideoQueueSize()
{
    unsigned int count = 0;
    pthread_mutex_lock(&videoMutex);//上视频锁
    count = video_packet_queue.size();//获取视频队列长度
    pthread_mutex_unlock(&videoMutex);//解视频锁
    return count;
}
