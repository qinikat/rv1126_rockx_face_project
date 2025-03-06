#include <pthread.h>
#include <string.h>
#include "rkmedia_container.h"

RV1126_ALL_CONTAINER all_containers;
pthread_mutex_t all_containers_mutex;

int init_all_container_function()
{
    pthread_mutex_init(&all_containers_mutex, NULL);
    memset(&all_containers, 0, sizeof(all_containers));

    return 0;
}

int set_vi_container(unsigned int index, RV1126_VI_CONTAINTER *vi_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    all_containers.vi_containers[index] = *vi_container;
    pthread_mutex_unlock(&all_containers_mutex);

    return 0;
}

int get_vi_container(unsigned int index, RV1126_VI_CONTAINTER *vi_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    *vi_container = all_containers.vi_containers[index];
    pthread_mutex_unlock(&all_containers_mutex);

    return 0;
}

int set_ai_container(unsigned int index, RV1126_AI_CONTAINTER *ai_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    all_containers.ai_containers[index] = *ai_container;
    pthread_mutex_unlock(&all_containers_mutex);

    return 0;
}

int get_ai_container(unsigned int index, RV1126_AI_CONTAINTER *ai_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    *ai_container = all_containers.ai_containers[index];
    pthread_mutex_unlock(&all_containers_mutex);

    return 0;
}


int set_venc_container(unsigned int index, RV1126_VENC_CONTAINER *venc_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    all_containers.venc_containers[index] = *venc_container;
    pthread_mutex_unlock(&all_containers_mutex);
    return 0;
}

int get_venc_container(unsigned int index, RV1126_VENC_CONTAINER *venc_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    *venc_container = all_containers.venc_containers[index];
    pthread_mutex_unlock(&all_containers_mutex);

    return 0;
}

int set_aenc_container(unsigned int index, RV1126_AENC_CONTAINER *aenc_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    all_containers.aenc_containers[index] = *aenc_container;
    pthread_mutex_unlock(&all_containers_mutex);
    return 0;
}

int get_aenc_container(unsigned int index, RV1126_AENC_CONTAINER *aenc_container)
{
    pthread_mutex_lock(&all_containers_mutex);
    *aenc_container = all_containers.aenc_containers[index];
    pthread_mutex_unlock(&all_containers_mutex);
    return 0;
}

