#include "ffmpeg_audio_queue.h"

//AUDIO队列的构造器，包含mutex的初始化和条件变量初始化
AUDIO_QUEUE::AUDIO_QUEUE()
{
    pthread_mutex_init(&audioMutex, NULL);//mutex的初始化
    pthread_cond_init(&audioCond, NULL);//条件变量初始化
}

//AUDIO队列的析构函数，锁的销毁和条件变量的销毁
AUDIO_QUEUE ::~AUDIO_QUEUE()
{
    pthread_mutex_destroy(&audioMutex);
    pthread_cond_destroy(&audioCond);
}

//AUDIO_QUEUE的插入音频队列操作
int AUDIO_QUEUE::putAudioPacketQueue(audio_data_packet *audio_packet)
{
    pthread_mutex_lock(&audioMutex);//上音频锁
    audio_packet_queue.push(audio_packet);//向音频队列插入audio_data_packet包
    pthread_cond_broadcast(&audioCond);//唤醒视音频队列
    pthread_mutex_unlock(&audioMutex);//解音频锁
    return 0;
}

//AUDIO_QUEUE取出音频包
audio_data_packet *AUDIO_QUEUE::getAudioPacketQueue()
{
    pthread_mutex_lock(&audioMutex);//上音频锁
    while (audio_packet_queue.size() == 0)
    {
        pthread_cond_wait(&audioCond, &audioMutex);//当音频队列没有数据的时候，等待被唤醒
    }
    audio_data_packet *item = audio_packet_queue.front();//把音频数据包移到最前面
    audio_packet_queue.pop();//pop取出音频数据并删除
    pthread_mutex_unlock(&audioMutex);//解音频锁
    return item;
}

//AUDIO_QUEUE音频队列长度
int AUDIO_QUEUE::getAudioPacketQueueSize()
{
    unsigned int count = 0;
    pthread_mutex_lock(&audioMutex);//上音频锁
    count = audio_packet_queue.size();//获取音频队列长度
    pthread_mutex_unlock(&audioMutex);//解音频锁
    return count;
}
