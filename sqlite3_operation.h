#ifndef _SQLITE3_OPERATION_H
#define _SQLITE3_OPERATION_H

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream> 
#include <fstream> 


#include <map>
#include <vector>
#include <uchar.h>
#include "rockx.h"

using namespace std;

typedef struct People
{
    string people_name;
    vector<char> images;

    bool operator <(const People& other) const
    {
        return false;
    }

}People;



int Connection_sqlite3DataBase();


void insert_face_data_toDataBase(const char *name, float feature[512], int featureSize, uint8_t * image_data, int image_length);

map<string, rockx_face_feature_t> QueryFaceFeature();

map<People, rockx_face_feature_t> QueryPeopleData();


//vector<char> QueryImageData();

struct People QueryImageData();


#endif

