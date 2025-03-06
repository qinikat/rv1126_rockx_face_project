#include "rockx_data_manage.h"
#include <string>

rockx_object_array_t face_array_bak;
string predictName;
rockx_face_result_t rockx_face_result;
rockx_face_result_group_t face_result_group;

People rockx_people;

pthread_mutex_t g_face_array_mutex;
pthread_mutex_t g_face_predict_mutex;
pthread_mutex_t g_face_result_mutex;
pthread_mutex_t g_face_people_mutex;

int init_all_rockx_face_data()
{
    pthread_mutex_init(&g_face_array_mutex, NULL);
    pthread_mutex_init(&g_face_predict_mutex, NULL);
    pthread_mutex_init(&g_face_result_mutex, NULL);
    pthread_mutex_init(&g_face_people_mutex, NULL);
}

void set_rockx_face_array(rockx_object_array_t face_array)
{
    pthread_mutex_lock(&g_face_array_mutex);
    face_array_bak = face_array;
    pthread_mutex_unlock(&g_face_array_mutex);
}

rockx_object_array_t get_rockx_face_array()
{
    pthread_mutex_lock(&g_face_array_mutex);
    rockx_object_array_t face_array = face_array_bak;
    pthread_mutex_unlock(&g_face_array_mutex);
    return face_array;
}

void set_rockx_prdict_name(string name)
{
    pthread_mutex_lock(&g_face_array_mutex);
    predictName = name;
    pthread_mutex_unlock(&g_face_array_mutex);
}

string get_rockx_prdict_name()
{
    pthread_mutex_lock(&g_face_array_mutex);
    string predict_name = predictName;
    pthread_mutex_unlock(&g_face_array_mutex);
    return predict_name;
}

void set_rockx_face_resuslt_group(rockx_face_result_group_t face_result_group_t)
{
    pthread_mutex_lock(&g_face_result_mutex);
    face_result_group = face_result_group_t;
    pthread_mutex_unlock(&g_face_result_mutex);
}

rockx_face_result_group_t get_rockx_face_resuslt_group()
{
    pthread_mutex_lock(&g_face_result_mutex);
    rockx_face_result_group_t face_result_group_t = face_result_group;
    pthread_mutex_unlock(&g_face_result_mutex);
    return face_result_group_t;
}

void set_rockx_people(People people)
{
    pthread_mutex_lock(&g_face_people_mutex);
    rockx_people = people;
    pthread_mutex_unlock(&g_face_people_mutex);
}

People get_rockx_people()
{
    pthread_mutex_lock(&g_face_people_mutex);
    People rockx_people_t = rockx_people;
    pthread_mutex_unlock(&g_face_people_mutex);
    return rockx_people_t;
}