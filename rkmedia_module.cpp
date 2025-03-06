#include "rkmedia_module.h"

int rkmedia_function_init()
{
    RK_MPI_SYS_Init();
    return 0;
}

int rkmedia_vi_init(RV1126_VI_CONFIG *rv1126_vi_config)
{
    int ret;
    VI_CHN_ATTR_S vi_attr = rv1126_vi_config->attr;
    unsigned int id = rv1126_vi_config->id;
    //vi_attr.pcVideoNode = CMOS_DEVICE_NAME;//
    //初始化VI模块
    ret = RK_MPI_VI_SetChnAttr(CAMERA_ID, id, &vi_attr);
    //使能VI模块
    ret |= RK_MPI_VI_EnableChn(CAMERA_ID, id);
    if (ret != 0)
    {
        printf("create vi failed.....\n", ret);
        return -1;
    }
    return 0;
}

int rkmedia_ai_init(RV1126_AI_CONFIG *rv1126_ai_config)
{
    int ret;
    AI_CHN_ATTR_S ai_attr = rv1126_ai_config->attr;
    unsigned int id = rv1126_ai_config->id;
    //ai_attr.pcAudioNode = AUDIO_PATH;
    ret = RK_MPI_AI_SetChnAttr(id, &ai_attr); //设置AI属性
    ret = RK_MPI_AI_EnableChn(id); //使能AI模块
    if (ret != 0) 
    {
        printf("create ai failed...\n");
        return -1;
    }

    return 0;
}

//VENC的初始化
int rkmedia_venc_init(RV1126_VENC_CONFIG *rv1126_venc_config)
{
    int ret;
    VENC_CHN_ATTR_S venc_chn_attr = rv1126_venc_config->attr;
    unsigned int venc_id = rv1126_venc_config->id;
    ret = RK_MPI_VENC_CreateChn(rv1126_venc_config->id, &venc_chn_attr);
    if (ret != 0)
    {
        printf("create rv1126_venc_module failed\n");
        return -1;
    }
    else
    {
        printf("create rv1126_venc_module success\n");
    }

    return 0;
}

//初始化AENC参数
int rkmedia_aenc_init(RV1126_AENC_CONFIG *rv1126_aenc_config)
{
    int ret;
    AENC_CHN_ATTR_S aenc_chn_attr = rv1126_aenc_config->attr;
    unsigned int aenc_id = rv1126_aenc_config->id;

    //创建AENC层
    ret = RK_MPI_AENC_CreateChn(aenc_id, &aenc_chn_attr);
    if (ret != 0)
    {
        printf("create aenc failed: %d\n");
        return -1;
    }

    return 0;
}
