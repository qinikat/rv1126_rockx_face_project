#ifndef _RV1126_PUBLIC_H
#define _RV1126_PUBLIC_H

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sample_common.h"
#include "rkmedia_api.h"
#include "rkmedia_venc.h"

#define NB_SAMPLES 1024

#define CAMERA_ID 0
#define AI_CMOS_DEVICE_NAME "rkispp_scale0" 
#define SHOW_CMOS_DEVICE_NAME "rkispp_scale1" 
#define AUDIO_PATH "default"

typedef struct
{
    unsigned int id;
    VI_CHN_ATTR_S attr;
} RV1126_VI_CONFIG;

typedef struct
{
    unsigned int id;
    AI_CHN_ATTR_S attr;
} RV1126_AI_CONFIG;


typedef struct
{
    unsigned int id;
    VENC_CHN_ATTR_S attr;
} RV1126_VENC_CONFIG;

typedef struct
{
    unsigned int id;
    AENC_CHN_ATTR_S attr;
} RV1126_AENC_CONFIG;

#endif
