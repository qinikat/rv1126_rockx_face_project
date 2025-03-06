#include "rkmedia_module_function.h"
#include "rkmedia_assignment_manage.h"
#include "rkmedia_config_public.h"
#include "rkmedia_module.h"
#include "rkmedia_container.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include <sys/time.h>

#define FILE_IMAGE_LENGTH (64 * 1024)

static int get_align16_value(int input_value, int align)
{
    int handle_value = 0;
    if (align && (input_value % align))
        handle_value = (input_value / align + 1) * align;
    return handle_value;
}

int read_image(char *filename, char *buffer)
{
    if (filename == NULL || buffer == NULL)
        return -1;
    FILE *fp = fopen(filename, "rb"); // 以二进制模式读取该文件
    if (fp == NULL)
    {
        printf("fopen failed\n");
        return -2;
    }

    // 检测文件大小file
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int size = fread(buffer, 1, length, fp);
    if (size != length)
    {
        printf("fread failed:%d\n", size);
        return -3;
    }

    fclose(fp);
    return size;
}

static int get_cur_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);                       // 使用gettimeofday获取当前系统时间
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000); // 利用struct timeval结构体将时间转换为ms
}

int init_rkmedia_module_function()
{
    rkmedia_function_init();

    RV1126_VI_CONFIG rockx_rkmedia_vi_config;
    memset(&rockx_rkmedia_vi_config, 0, sizeof(rockx_rkmedia_vi_config));
    rockx_rkmedia_vi_config.id = 0;
    rockx_rkmedia_vi_config.attr.pcVideoNode = AI_CMOS_DEVICE_NAME;   // VIDEO视频节点路径,
    rockx_rkmedia_vi_config.attr.u32BufCnt = 3;                    // VI捕获视频缓冲区计数，默认是3
    rockx_rkmedia_vi_config.attr.u32Width = 1920;                  // 视频输入的宽度，一般和CMOS摄像头或者外设的宽度一致
    rockx_rkmedia_vi_config.attr.u32Height = 1080;                 // 视频输入的高度，一般和CMOS摄像头或者外设的高度一致
    rockx_rkmedia_vi_config.attr.enPixFmt = IMAGE_TYPE_NV12;       // 视频输入的图像格式，默认是NV12(IMAGE_TYPE_NV12)
    rockx_rkmedia_vi_config.attr.enBufType = VI_CHN_BUF_TYPE_MMAP; // VI捕捉视频的类型
    rockx_rkmedia_vi_config.attr.enWorkMode = VI_WORK_MODE_NORMAL; // VI的工作模式，默认是NORMAL(VI_WORK_MODE_NORMAL)
    int ret = rkmedia_vi_init(&rockx_rkmedia_vi_config);           // 初始化VI工作
    if (ret != 0)
    {
        printf("rockx_vi init error\n");
    }
    else
    {
        printf("rockx_vi init success\n");
        RV1126_VI_CONTAINTER vi_container;
        vi_container.id = 0;
        vi_container.vi_id = rockx_rkmedia_vi_config.id;
        set_vi_container(0, &vi_container); // 设置VI容器
    }


    RV1126_VI_CONFIG show_rkmedia_vi_config;
    memset(&show_rkmedia_vi_config, 0, sizeof(show_rkmedia_vi_config));
    show_rkmedia_vi_config.id = 1;
    show_rkmedia_vi_config.attr.pcVideoNode = SHOW_CMOS_DEVICE_NAME;   // VIDEO视频节点路径,
    show_rkmedia_vi_config.attr.u32BufCnt = 3;                    // VI捕获视频缓冲区计数，默认是3
    show_rkmedia_vi_config.attr.u32Width = 1920;                  // 视频输入的宽度，一般和CMOS摄像头或者外设的宽度一致
    show_rkmedia_vi_config.attr.u32Height = 1080;                 // 视频输入的高度，一般和CMOS摄像头或者外设的高度一致
    show_rkmedia_vi_config.attr.enPixFmt = IMAGE_TYPE_NV12;       // 视频输入的图像格式，默认是NV12(IMAGE_TYPE_NV12)
    show_rkmedia_vi_config.attr.enBufType = VI_CHN_BUF_TYPE_MMAP; // VI捕捉视频的类型
    show_rkmedia_vi_config.attr.enWorkMode = VI_WORK_MODE_NORMAL; // VI的工作模式，默认是NORMAL(VI_WORK_MODE_NORMAL)
    ret = rkmedia_vi_init(&show_rkmedia_vi_config);           // 初始化VI工作
    if (ret != 0)
    {
        printf("show_vi init error\n");
    }
    else
    {
        printf("show_vi init success\n");
        RV1126_VI_CONTAINTER vi_container;
        vi_container.id = 1;
        vi_container.vi_id = show_rkmedia_vi_config.id;
        set_vi_container(1, &vi_container); // 设置VI容器
    }
 
    RV1126_VENC_CONFIG rkmedia_venc_config = {0};
    memset(&rkmedia_venc_config, 0, sizeof(rkmedia_venc_config));
    rkmedia_venc_config.id = 0;
    rkmedia_venc_config.attr.stVencAttr.enType = RK_CODEC_TYPE_H264;          // 编码器协议类型
    rkmedia_venc_config.attr.stVencAttr.imageType = IMAGE_TYPE_NV12;          // 输入图像类型
    rkmedia_venc_config.attr.stVencAttr.u32PicWidth = 1920;                   // 编码图像宽度
    rkmedia_venc_config.attr.stVencAttr.u32PicHeight = 1080;                  // 编码图像高度
    rkmedia_venc_config.attr.stVencAttr.u32VirWidth = 1920;                   // 编码图像虚宽度，一般来说u32VirWidth和u32PicWidth是一致的
    rkmedia_venc_config.attr.stVencAttr.u32VirHeight = 1080;                  // 编码图像虚高度，一般来说u32VirHeight和u32PicHeight是一致的
    rkmedia_venc_config.attr.stVencAttr.u32Profile = 66;                      // 编码等级H.264: 66: Baseline; 77:Main Profile; 100:High Profile; H.265: default:Main; Jpege/MJpege: default:Baseline(编码等级的作用主要是改变画面质量，66的画面质量最差利于网络传输，100的质量最好)

    rkmedia_venc_config.attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;        // 编码器码率控制模式
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.u32Gop = 25;                  // GOPSIZE:关键帧间隔
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.u32BitRate = 1920 * 1080 * 3; // 码率
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;      // 目的帧率分子:填的是1固定
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 25;     // 目的帧率分母:填的是25固定
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;       // 源头帧率分子:填的是1固定
    rkmedia_venc_config.attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;      // 源头帧率分母:填的是25固定

    ret = rkmedia_venc_init(&rkmedia_venc_config);                            // VENC模块的初始化
    if (ret != 0)
    {
        printf("venc init error\n");
    }
    else
    {
        RV1126_VENC_CONTAINER venc_container;
        venc_container.id = 0;
        venc_container.venc_id = rkmedia_venc_config.id;
        set_venc_container(0, &venc_container);
        printf("venc init success\n");
    }


    ret = RK_MPI_VI_StartStream(CAMERA_ID, 0);
    if(ret)
    {
        printf("RK_MPI_VI_StartStream_0 Failed....\n");
    }

    ret = RK_MPI_VI_StartStream(CAMERA_ID, 1);
    if(ret)
    {
        printf("RK_MPI_VI_StartStream_1 Failed....\n");
    }



    return 0;
}
