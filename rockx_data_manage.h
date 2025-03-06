#ifndef _ROCKX_DATA_MANAGE_H
#define _ROCKX_DATA_MANAGE_H

#include "rockx.h"
#include <pthread.h>
#include <stdio.h>
#include <string>
#include "sqlite3_operation.h"

using namespace std;

typedef struct ROCKX_FACE_BOX
{
    int left;
    int top;
    int right;
    int bottom;
}ROCKX_FACE_BOX;

typedef struct _rockx_face_result_t
{
    string predictName;
    ROCKX_FACE_BOX face_box;

}rockx_face_result_t;


typedef struct _rockx_face_result_group
{
    //int count;
    rockx_object_array_t rockx_face_array;
    string predictName;
    //rockx_face_result_t rockx_face_results[128];

} rockx_face_result_group_t;

int init_all_rockx_face_data();
void set_rockx_face_array(rockx_object_array_t face_array);
rockx_object_array_t get_rockx_face_array();

void set_rockx_prdict_name(string name);
string get_rockx_prdict_name();

void set_rockx_people(People people);
People get_rockx_people();

void set_rockx_face_resuslt_group(rockx_face_result_group_t face_result_group_t);
rockx_face_result_group_t get_rockx_face_resuslt_group();

#endif