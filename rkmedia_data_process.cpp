// #include "opencv_queue.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include "rkmedia_data_process.h"
#include "ffmpeg_video_queue.h"
#include "ffmpeg_audio_queue.h"
#include "rkmedia_module.h"
#include "rkmedia_ffmpeg_config.h"
#include "rkmedia_data_process.h"
#include "map_manage.h"
#include "rockx_data_manage.h"
#include "rockx.h"
#include "cJSON/cJSON.h"

using namespace cv;

extern VIDEO_QUEUE *high_video_queue;
//extern VIDEO_QUEUE *low_video_queue;

int can_read_pic = 0;

int is_upload_to_mqtt_flag = 0;
People mqtt_people;

void nv12ToBgr(uint8_t *data, int width, int height, uint8_t *bgr)
{
    cv::Mat yuv(height + height / 2, width, CV_8UC1, (uchar *)data);
    cv::Mat bgrMat(height, width, CV_8UC3, bgr);
    cv::cvtColor(yuv, bgrMat, cv::COLOR_YUV2BGR_NV12);
}

// 从RV1126视频编码数据赋值到FFMPEG的Video AVPacket中
AVPacket *get_ffmpeg_video_avpacket(AVPacket *pkt)
{
    video_data_packet_t *video_data_packet = high_video_queue->getVideoPacketQueue(); // 从视频队列获取数据

    if (video_data_packet != NULL)
    {
        /*
     重新分配给定的缓冲区
   1.  如果入参的 AVBufferRef 为空，直接调用 av_realloc 分配一个新的缓存区，并调用 av_buffer_create 返回一个新的 AVBufferRef 结构；
   2.  如果入参的缓存区长度和入参 size 相等，直接返回 0；
   3.  如果对应的 AVBuffer 设置了 BUFFER_FLAG_REALLOCATABLE 标志，或者不可写，再或者 AVBufferRef data 字段指向的数据地址和 AVBuffer 的 data 地址不同，递归调用 av_buffer_realloc 分配一个新
的 buffer，并将 data 拷贝过去；
   4.  不满足上面的条件，直接调用 av_realloc 重新分配缓存区。
 */
        int ret = av_buffer_realloc(&pkt->buf, video_data_packet->video_frame_size + 70);
        if (ret < 0)
        {
            return NULL;
        }
        pkt->size = video_data_packet->video_frame_size;                                        // rv1126的视频长度赋值到AVPacket Size
        memcpy(pkt->buf->data, video_data_packet->buffer, video_data_packet->video_frame_size); // rv1126的视频数据赋值到AVPacket data
        pkt->data = pkt->buf->data;                                                             // 把pkt->buf->data赋值到pkt->data
        pkt->flags |= AV_PKT_FLAG_KEY;                                                          // 默认flags是AV_PKT_FLAG_KEY
        if (video_data_packet != NULL)
        {
            free(video_data_packet);
            video_data_packet = NULL;
        }

        return pkt;
    }
    else
    {
        return NULL;
    }
}

#if 0
AVPacket *get_low_ffmpeg_video_avpacket(AVPacket *pkt)
{
    video_data_packet_t *video_data_packet = low_video_queue->getVideoPacketQueue(); // 从视频队列获取数据

    if (video_data_packet != NULL)
    {
        /*
     重新分配给定的缓冲区
   1.  如果入参的 AVBufferRef 为空，直接调用 av_realloc 分配一个新的缓存区，并调用 av_buffer_create 返回一个新的 AVBufferRef 结构；
   2.  如果入参的缓存区长度和入参 size 相等，直接返回 0；
   3.  如果对应的 AVBuffer 设置了 BUFFER_FLAG_REALLOCATABLE 标志，或者不可写，再或者 AVBufferRef data 字段指向的数据地址和 AVBuffer 的 data 地址不同，递归调用 av_buffer_realloc 分配一个新
的 buffer，并将 data 拷贝过去；
   4.  不满足上面的条件，直接调用 av_realloc 重新分配缓存区。
 */
        int ret = av_buffer_realloc(&pkt->buf, video_data_packet->video_frame_size + 70);
        if (ret < 0)
        {
            return NULL;
        }
        pkt->size = video_data_packet->video_frame_size;                                        // rv1126的视频长度赋值到AVPacket Size
        memcpy(pkt->buf->data, video_data_packet->buffer, video_data_packet->video_frame_size); // rv1126的视频数据赋值到AVPacket data
        pkt->data = pkt->buf->data;                                                             // 把pkt->buf->data赋值到pkt->data
        pkt->flags |= AV_PKT_FLAG_KEY;                                                          // 默认flags是AV_PKT_FLAG_KEY
        if (video_data_packet != NULL)
        {
            free(video_data_packet);
            video_data_packet = NULL;
        }

        return pkt;
    }
    else
    {
        return NULL;
    }
}
#endif

int write_ffmpeg_avpacket(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /*将输出数据包时间戳值从编解码器重新调整为流时基 */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    return av_interleaved_write_frame(fmt_ctx, pkt);
}

int deal_video_avpacket(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVPacket *video_packet = get_ffmpeg_video_avpacket(ost->packet); // 从RV1126视频编码数据赋值到FFMPEG的Video AVPacket中
    if (video_packet != NULL)
    {
        video_packet->pts = ost->next_timestamp++; // VIDEO_PTS按照帧率进行累加
    }

    ret = write_ffmpeg_avpacket(oc, &c->time_base, ost->stream, video_packet); // 向复合流写入视频数据
    if (ret != 0)
    {
        printf("write video avpacket error");
        return -1;
    }

    return 0;
}

/*int deal_low_video_avpacket(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVPacket *video_packet = get_low_ffmpeg_video_avpacket(ost->packet); // 从RV1126视频编码数据赋值到FFMPEG的Video AVPacket中
    if (video_packet != NULL)
    {
        video_packet->pts = ost->next_timestamp++; // VIDEO_PTS按照帧率进行累加
    }

    ret = write_ffmpeg_avpacket(oc, &c->time_base, ost->stream, video_packet); // 向复合流写入视频数据
    if (ret != 0)
    {
        printf("write video avpacket error");
        return -1;
    }

    return 0;
}*/

int nv12_border(char *pic, int pic_w, int pic_h, int rect_x, int rect_y,
                int rect_w, int rect_h, int R, int G, int B)
{
    /* Set up the rectangle border size */
    const int border = 5;
    /* RGB convert YUV */
    int Y, U, V;
    Y = 0.299 * R + 0.587 * G + 0.114 * B;
    U = -0.1687 * R + 0.3313 * G + 0.5 * B + 128;
    V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;
    /* Locking the scope of rectangle border range */
    int j, k;
    for (j = rect_y; j < rect_y + rect_h; j++)
    {
        for (k = rect_x; k < rect_x + rect_w; k++)
        {
            if (k < (rect_x + border) || k > (rect_x + rect_w - border) ||
                j < (rect_y + border) || j > (rect_y + rect_h - border))
            {
                /* Components of YUV's storage address index */
                int y_index = j * pic_w + k;
                int u_index =
                    (y_index / 2 - pic_w / 2 * ((j + 1) / 2)) * 2 + pic_w * pic_h;
                int v_index = u_index + 1;
                /* set up YUV's conponents value of rectangle border */
                pic[y_index] = Y;
                pic[u_index] = U;
                pic[v_index] = V;
            }
        }
    }

    return 0;
}

void *rockx_vi_handle_thread(void *args)
{
    pthread_detach(pthread_self());

    S_THREAD_MAP thread_map;
    get_thread_map(0, &thread_map);

    map<People, rockx_face_feature_t> database_face_map = thread_map.thread_people_map;
    map<People, rockx_face_feature_t>::iterator database_iter;

    MEDIA_BUFFER src_mb = NULL;

    rockx_module_t data_version;
    data_version = ROCKX_MODULE_FACE_DETECTION_V2;

    rockx_ret_t rockx_ret;
    rockx_handle_t face_det_handle;
    rockx_handle_t face_recognize_handle;
    rockx_handle_t face_5landmarks_handle;
    rockx_handle_t face_masks_det_handle;

    rockx_config_t *config = rockx_create_config();
    rockx_add_config(config, ROCKX_CONFIG_DATA_PATH, "/userdata/rockx_data/");

    rockx_ret = rockx_create(&face_det_handle, data_version, config, sizeof(rockx_config_t));
    if (rockx_ret != ROCKX_RET_SUCCESS)
    {
        printf("init face_detect error %d\n", rockx_ret);
        return NULL;
    }

    rockx_ret = rockx_create(&face_recognize_handle, ROCKX_MODULE_FACE_RECOGNIZE, config, sizeof(rockx_config_t));
    if (rockx_ret != ROCKX_RET_SUCCESS)
    {
        printf("init face_recognize error %d\n", rockx_ret);
        return NULL;
    }

    rockx_ret = rockx_create(&face_5landmarks_handle, ROCKX_MODULE_FACE_LANDMARK_5, config, 0);
    if (rockx_ret != ROCKX_RET_SUCCESS)
    {
        printf("init rockx module ROCKX_MODULE_FACE_LANDMARK_68 error %d\n",
               rockx_ret);
    }

    /*rockx_ret = rockx_create(&face_masks_det_handle,
                             ROCKX_MODULE_FACE_MASKS_DETECTION, config, 0);
    if (rockx_ret != ROCKX_RET_SUCCESS)
    {
        printf("init rockx module ROCKX_MODULE_FACE_LANDMARK_68 error %d\n",
               rockx_ret);
    }*/

    rockx_image_t input_image;
    input_image.width = 1920;
    input_image.height = 1080;
    input_image.pixel_format = ROCKX_PIXEL_FORMAT_YUV420SP_NV12;
    int ret;

    bool is_recognize = false;
    string predict;
    int face_count = 0;

    uint8_t *bgr = new uint8_t[1920 * 1080 * 3];
    int count = 0;

    People people;

    while (1)
    {
        src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, 0, -1);
        if (!src_mb)
        {
            printf("");
            break;
        }

        input_image.size = RK_MPI_MB_GetSize(src_mb);
        input_image.data = (unsigned char *)RK_MPI_MB_GetPtr(src_mb); // 从指定的MEDIA_BUFFER中获取缓冲区数据指针

        rockx_object_array_t face_array;
        memset(&face_array, 0, sizeof(face_array));

        // 开始人脸检测
        rockx_ret = rockx_face_detect(face_det_handle, &input_image, &face_array, NULL);
        if (rockx_ret != ROCKX_RET_SUCCESS)
        {
            printf("rockx_face_detect ERROR %d\n", rockx_ret);
        }

        set_rockx_face_array(face_array);
        // printf("face_count = %d\n", face_array.count);

        if (face_array.count > 0)
        {
            for (int i = 0; i < face_array.count; i++)
            {
                if (1)
                {
                    int is_false_face;
                    // 进行人脸过滤处理
                    ret = rockx_face_filter(face_5landmarks_handle, &input_image,
                                            &face_array.object[i].box, &is_false_face);
                    if (ret != ROCKX_RET_SUCCESS)
                    {
                        printf("rockx_face_filter error %d\n", ret);
                    }
                    if (is_false_face)
                        continue;
                }

#if 1
                // 人脸检测结果（包括人脸、车牌、头部、物体等）变量定义
                rockx_object_t max_face;
                rockx_object_t cur_face = face_array.object[i];
                // 进行人脸区域计算处理操作
                int cur_face_box_area = (cur_face.box.right - cur_face.box.left) *
                                        (cur_face.box.bottom - cur_face.box.top);
                int max_face_box_area = (max_face.box.right - max_face.box.left) *
                                        (max_face.box.bottom - max_face.box.top);
                if (cur_face_box_area > max_face_box_area)
                {
                    max_face = cur_face;
                }

                // 检测输出处理
                rockx_image_t out_img;
                memset(&out_img, 0, sizeof(rockx_image_t));
                // 进行面部矫正对齐
                ret = rockx_face_align(face_5landmarks_handle, &input_image, &(max_face.box), NULL, &out_img);
                if (ret != ROCKX_RET_SUCCESS)
                {
                    printf("face_align failed\n");
                }

                rockx_face_feature_t rv1126_feature;
                rockx_face_recognize(face_recognize_handle, &out_img, &rv1126_feature);

                for (database_iter = database_face_map.begin();
                     database_iter != database_face_map.end(); database_iter++)
                {
                    float similarity;
                    ret = rockx_face_feature_similarity(&database_iter->second,
                                                        &rv1126_feature, &similarity);
                    printf("simple_value = %lf\n", similarity);
                    if (similarity <= 1.0)
                    {
                        is_recognize = true;
                        break;
                    }
                    else
                    {
                        is_recognize = false;
                        continue;
                    }
                }

                if (is_recognize == true)
                {
                    people = database_iter->first;
                    printf("people_name = %s\n", people.people_name.c_str());
                }
                set_rockx_people(people);
#endif
            }
        }

        RK_MPI_MB_ReleaseBuffer(src_mb);
        src_mb = NULL;
    }
}

void *show_vi_thread(void *args)
{
    pthread_detach(pthread_self());
    MEDIA_BUFFER mb = NULL;

    float x_rate = (float)1920 / 1920;
    float y_rate = (float)1920 / 1080;

    Point pointer;
    pointer.x = 300;
    pointer.y = 300;

    while (1)
    {
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, 1, -1);
        if (!mb)
        {
            printf("RK_MPI_SYS_GetMediaBuffer Break.....\n");
            break;
        }

        Mat rv1126_mat = Mat(HEIGHT, WIDTH, CV_8UC1, RK_MPI_MB_GetPtr(mb));
        rockx_object_array_t face_array = get_rockx_face_array();
        People people = get_rockx_people();
        // string face_count_str = std::to_string(face_array.count);
        // cv::putText(rv1126_mat, face_count_str, pointer, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 255), 3);

        for (int i = 0; i < face_array.count; i++)
        {
            int x = face_array.object[i].box.left * x_rate;
            int y = face_array.object[i].box.top * y_rate;
            int w = (face_array.object[i].box.right - face_array.object[i].box.left) * x_rate;
            int h = (face_array.object[i].box.bottom - face_array.object[i].box.top) * y_rate;

            if (x < 0)
                x = 0;
            if (y < 0)
                y = 0;

            while ((uint32_t)(x + w) >= 1920)
            {
                w -= 16;
            }
            while ((uint32_t)(y + h) >= 1080)
            {
                h -= 16;
            }

            Scalar color(255, 0, 255);
            Rect boundingBox(x, y, 500, 500);
            int thickness = 3;
            rectangle(rv1126_mat, boundingBox, color, thickness);

            int baseline;
            Size text_size = getTextSize(people.people_name, 2, 2, 2, &baseline);
            Point origin;
            origin.x = rv1126_mat.cols / 4 - text_size.width / 4;
            origin.y = rv1126_mat.rows / 4 + text_size.height / 4;
            cv::putText(rv1126_mat, people.people_name, origin, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 255), 3);
        }

        RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC, 0, mb);
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    return NULL;
}

void *camera_venc_thread(void *args)
{
    pthread_detach(pthread_self());
    MEDIA_BUFFER mb = NULL;

    VENC_PROC_PARAM venc_arg = *(VENC_PROC_PARAM *)args;
    free(args);

    printf("video_venc_thread...\n");

    while (1)
    {
        // 从指定通道中获取VENC数据
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC, venc_arg.vencId, -1);
        if (!mb)
        {
            printf("high_get venc media buffer error\n");
            break;
        }

        // int naluType = RK_MPI_MB_GetFlag(mb);
        // 分配video_data_packet_t结构体
        video_data_packet_t *video_data_packet = (video_data_packet_t *)malloc(sizeof(video_data_packet_t));
        // 把VENC视频缓冲区数据传输到video_data_packet的buffer中
        memcpy(video_data_packet->buffer, RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb));
        // 把VENC的长度赋值给video_data_packet的video_frame_size中
        video_data_packet->video_frame_size = RK_MPI_MB_GetSize(mb);
        // video_data_packet->frame_flag = naluType;
        // 入到视频压缩队列
        high_video_queue->putVideoPacketQueue(video_data_packet);
        // printf("#naluType = %d \n", naluType);
        // 释放VENC资源
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    MPP_CHN_S vi_channel;
    MPP_CHN_S venc_channel;

    vi_channel.enModId = RK_ID_VI;
    vi_channel.s32ChnId = 0;

    venc_channel.enModId = RK_ID_VENC;
    venc_channel.s32ChnId = venc_arg.vencId;

    int ret;
    ret = RK_MPI_SYS_UnBind(&vi_channel, &venc_channel);
    if (ret != 0)
    {
        printf("VI UnBind failed \n");
    }
    else
    {
        printf("Vi UnBind success\n");
    }

    ret = RK_MPI_VENC_DestroyChn(0);
    if (ret)
    {
        printf("Destroy Venc error! ret=%d\n", ret);
        return 0;
    }
    // destroy vi
    ret = RK_MPI_VI_DisableChn(0, 0);
    if (ret)
    {
        printf("Disable Chn Venc error! ret=%d\n", ret);
        return 0;
    }

    return NULL;
}

void *get_rga_thread(void *args)
{
    MEDIA_BUFFER mb = NULL;

    while (1)
    {
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1); // 获取RGA的数据
        if (!mb)
        {
            break;
        }

        RK_MPI_SYS_SendMediaBuffer(RK_ID_VENC, 1, mb); //
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    return NULL;
}

#if 0
void *low_camera_venc_thread(void *args)
{
    pthread_detach(pthread_self());
    MEDIA_BUFFER mb = NULL;

    VENC_PROC_PARAM venc_arg = *(VENC_PROC_PARAM *)args;
    free(args);

    printf("low_video_venc_thread...\n");

    while (1)
    {
        // 从指定通道中获取VENC数据
        // mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC, venc_arg.vencId, -1);
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VENC, 1, -1);
        if (!mb)
        {
            printf("low_venc break....\n");
            break;
        }

        // int naluType = RK_MPI_MB_GetFlag(mb);
        // 分配video_data_packet_t结构体
        video_data_packet_t *video_data_packet = (video_data_packet_t *)malloc(sizeof(video_data_packet_t));
        // 把VENC视频缓冲区数据传输到video_data_packet的buffer中
        memcpy(video_data_packet->buffer, RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb));
        // 把VENC的长度赋值给video_data_packet的video_frame_size中
        video_data_packet->video_frame_size = RK_MPI_MB_GetSize(mb);
        // video_data_packet->frame_flag = naluType;
        // 入到视频压缩队列
        low_video_queue->putVideoPacketQueue(video_data_packet);
        // printf("#naluType = %d \n", naluType);
        // 释放VENC资源
        RK_MPI_MB_ReleaseBuffer(mb);
    }

    return NULL;
}
#endif

// 音视频合成推流线程
void *video_push_thread(void *args)
{
    pthread_detach(pthread_self());
    RKMEDIA_FFMPEG_CONFIG ffmpeg_config = *(RKMEDIA_FFMPEG_CONFIG *)args;
    free(args);
    AVOutputFormat *fmt = NULL;
    int ret;

    while (1)
    {
        ret = deal_video_avpacket(ffmpeg_config.oc, &ffmpeg_config.video_stream); // 处理FFMPEG视频数据
        if (ret == -1)
        {
            printf("deal_video_avpacket error\n");
            break;
        }
    }

    av_write_trailer(ffmpeg_config.oc);                         // 写入AVFormatContext的尾巴
    free_stream(ffmpeg_config.oc, &ffmpeg_config.video_stream); // 释放VIDEO_STREAM的资源
    free_stream(ffmpeg_config.oc, &ffmpeg_config.audio_stream); // 释放AUDIO_STREAM的资源
    avio_closep(&ffmpeg_config.oc->pb);                         // 释放AVIO资源
    avformat_free_context(ffmpeg_config.oc);                    // 释放AVFormatContext资源
    return NULL;
}

#if 0
void *low_video_push_thread(void *args)
{
    pthread_detach(pthread_self());
    RKMEDIA_FFMPEG_CONFIG ffmpeg_config = *(RKMEDIA_FFMPEG_CONFIG *)args;
    free(args);
    AVOutputFormat *fmt = NULL;
    int ret;

    while (1)
    {
        ret = deal_low_video_avpacket(ffmpeg_config.oc, &ffmpeg_config.video_stream); // 处理FFMPEG视频数据
        if (ret == -1)
        {
            printf("deal_video_avpacket error\n");
            break;
        }
    }

    av_write_trailer(ffmpeg_config.oc);                         // 写入AVFormatContext的尾巴
    free_stream(ffmpeg_config.oc, &ffmpeg_config.video_stream); // 释放VIDEO_STREAM的资源
    free_stream(ffmpeg_config.oc, &ffmpeg_config.audio_stream); // 释放AUDIO_STREAM的资源
    avio_closep(&ffmpeg_config.oc->pb);                         // 释放AVIO资源
    avformat_free_context(ffmpeg_config.oc);                    // 释放AVFormatContext资源
    return NULL;
}
#endif

void *mqtt_cjson_upload_thread(void *args)
{
    int mqtt_flag = 0;

    while (1)
    {
        if (is_upload_to_mqtt_flag == 1)
        {
            mqtt_flag = 1;
        }

        if (mqtt_flag == 1 && is_upload_to_mqtt_flag == 1)
        {
            cJSON *json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "name", mqtt_people.people_name.c_str());
            cJSON_AddStringToObject(json, "image_data", mqtt_people.images.data());
            // cJSON_AddStringToObject(json, "image_length", people.images.data());
            char *str_print = cJSON_Print(json);
            printf("JSON_Str = %s\n", str_print);
            mqtt_flag = 0;
        }
        else
        {
            printf("No Upload....\n");
        }
    }
}