#ifndef _MAP_MANAGE_H
#define _MAP_MANAGE_H

#include <map>
#include <string>
#include <iostream>
#include "rockx.h"
#include "sqlite3_operation.h"

#define MAX_MAP_NUM (3)
using namespace std;


class S_THREAD_MAP{

  public:
     int map_id;
     map<string, rockx_face_feature_t> thread_map;
     map<People, rockx_face_feature_t> thread_people_map;
};


int init_map_manage_function();
int set_map_id(unsigned int task_id);
unsigned int get_map_id();

int set_thread_map(unsigned int task_id, S_THREAD_MAP *stream);
unsigned int get_thread_map(unsigned int task_id, S_THREAD_MAP *stream);


#endif
