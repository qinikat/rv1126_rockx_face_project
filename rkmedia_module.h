#ifndef _RV1126_TASK_FUNCTION_H 
#define _RV1126_TASK_FUNCTION_H

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

#include "rkmedia_config_public.h"


int rkmedia_function_init();

int rkmedia_vi_init(RV1126_VI_CONFIG *vi_config);

int rkmedia_ai_init(RV1126_AI_CONFIG *ai_config);

int rkmedia_venc_init(RV1126_VENC_CONFIG *venc_config);

int rkmedia_aenc_init(RV1126_AENC_CONFIG *venc_config);


#endif