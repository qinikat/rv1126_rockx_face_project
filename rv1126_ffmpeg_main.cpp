#include "rkmedia_ffmpeg_config.h"
#include "rkmedia_container.h"
#include "ffmpeg_audio_queue.h"
#include "ffmpeg_video_queue.h"
#include "rkmedia_module_function.h"
#include "rkmedia_assignment_manage.h"
#include "map_manage.h"
#include "sqlite3_operation.h"
#include "rv1126_v4l2_camera.h"
#include "rockx.h"

VIDEO_QUEUE *high_video_queue = NULL;

void init_face_data()
{
    Connection_sqlite3DataBase();
    int task_id = 0;
    S_THREAD_MAP thread_map;
    map<People, rockx_face_feature_t> maps = QueryPeopleData();
    thread_map.thread_people_map = maps;
    set_thread_map(task_id, &thread_map);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Please Input ./rv1126_ffmpeg_main high_stream_type high_url_address . Notice URL_TYPE: 0-->FLV  1-->TS\n");
        return -1;
    }

    int high_protocol_type = atoi(argv[1]);
    char *high_network_address = argv[2];
    init_rkmedia_module_function();      // 初始化所有rkmedia的模块
    high_video_queue = new VIDEO_QUEUE(); // 初始化所有VIDEO队列
    init_face_data();    //初始化人脸数据                               
    init_rv1126_first_assignment(high_protocol_type, high_network_address); // 开启推流任务

    while (1)
    {
        sleep(20);
    }

    return 0;
}
