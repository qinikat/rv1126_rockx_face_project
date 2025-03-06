#include "rkmedia_assignment_manage.h"
#include "rkmedia_data_process.h"
#include "rkmedia_ffmpeg_config.h"
#include "rkmedia_module.h"

#include "rkmedia_ffmpeg_config.h"
#include "rkmedia_container.h"

int init_rv1126_first_assignment(int protocol_type, char * network_address)
{
    int ret;
    RKMEDIA_FFMPEG_CONFIG *ffmpeg_config = (RKMEDIA_FFMPEG_CONFIG *)malloc(sizeof(RKMEDIA_FFMPEG_CONFIG));
    if (ffmpeg_config == NULL)
    {
        printf("malloc ffmpeg_config failed\n");
    }
    ffmpeg_config->width = 1920;
    ffmpeg_config->height = 1080;
    ffmpeg_config->config_id = 0;
    ffmpeg_config->protocol_type = protocol_type;
    ffmpeg_config->video_codec = AV_CODEC_ID_H264;
    ffmpeg_config->audio_codec = AV_CODEC_ID_AAC;
    memcpy(ffmpeg_config->network_addr, network_address, strlen(network_address));
    //初始化ffmpeg输出模块
    init_rkmedia_ffmpeg_context(ffmpeg_config);

    pthread_t pid;
    VI_PROC_PARAM * vi_arg_params = (VI_PROC_PARAM *)malloc(sizeof(VI_PROC_PARAM));
    if(vi_arg_params == NULL)
    {
        printf("malloc venc arg error\n");
        free(vi_arg_params);
    }

    vi_arg_params->viId = 0;
    ret = pthread_create(&pid, NULL, rockx_vi_handle_thread, (void *)vi_arg_params);
    if (ret != 0)
    {
        printf("create camera_venc_thread failed\n");
    }


    VI_PROC_PARAM * show_vi_arg_params = (VI_PROC_PARAM *)malloc(sizeof(VI_PROC_PARAM));
    if(vi_arg_params == NULL)
    {
        printf("malloc venc arg error\n");
        free(vi_arg_params);
    }

    vi_arg_params->viId = 1;
    ret = pthread_create(&pid, NULL, show_vi_thread, (void *)show_vi_arg_params);
    if (ret != 0)
    {
        printf("create camera_venc_thread failed\n");
    }

    //VENC线程的参数
    VENC_PROC_PARAM *venc_arg_params = (VENC_PROC_PARAM *)malloc(sizeof(VENC_PROC_PARAM));
    if (venc_arg_params == NULL)
    {
        printf("malloc venc arg error\n");
        free(venc_arg_params);
    }

    venc_arg_params->vencId = 0;
    //创建VENC线程，获取摄像头编码数据
    ret = pthread_create(&pid, NULL, camera_venc_thread, (void *)venc_arg_params);
    if (ret != 0)
    {
        printf("create camera_venc_thread failed\n");
    }

    //创建VIDEO_PUSH线程
    ret = pthread_create(&pid, NULL, video_push_thread, (void *)ffmpeg_config);
    if (ret != 0)
    {
        printf("push_server_thread error\n");
    }

 
    return 0;
}
