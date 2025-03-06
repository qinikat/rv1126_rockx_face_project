#ifndef _RKMEDIA_CONTAINER_H
#define _RKMEDIA_CONTAINER_H

#define ALL_CONTAINER_NUM 20

#include <stdbool.h>
#include "rkmedia_config_public.h"

typedef struct
{
    unsigned int id;
    unsigned int vi_id;

}RV1126_VI_CONTAINTER;

typedef struct
{
    unsigned int id;
    unsigned int ai_id;

}RV1126_AI_CONTAINTER;

typedef struct
{
    unsigned int id;
    unsigned int venc_id;

}RV1126_VENC_CONTAINER;

typedef struct
{
    unsigned int id;
    unsigned int aenc_id;

}RV1126_AENC_CONTAINER;

typedef struct
{
    unsigned int container_id;
    RV1126_VI_CONTAINTER vi_containers[ALL_CONTAINER_NUM];
    RV1126_AI_CONTAINTER ai_containers[ALL_CONTAINER_NUM];

    RV1126_VENC_CONTAINER venc_containers[ALL_CONTAINER_NUM];
    RV1126_AENC_CONTAINER aenc_containers[ALL_CONTAINER_NUM];

}RV1126_ALL_CONTAINER;

int init_all_container_function();

int set_vi_container(unsigned int index, RV1126_VI_CONTAINTER *vi_container);
int get_vi_container(unsigned int index, RV1126_VI_CONTAINTER *vi_container);

int set_ai_container(unsigned int index, RV1126_AI_CONTAINTER *ai_container);
int get_ai_container(unsigned int index, RV1126_AI_CONTAINTER *ai_container);


int set_venc_container(unsigned int index, RV1126_VENC_CONTAINER *venc_container);
int get_venc_container(unsigned int index, RV1126_VENC_CONTAINER *venc_container);


int set_aenc_container(unsigned int index, RV1126_AENC_CONTAINER *aenc_container);
int get_aenc_container(unsigned int index, RV1126_AENC_CONTAINER *aenc_container);

#endif
