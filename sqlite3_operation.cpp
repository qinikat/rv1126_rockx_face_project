#include "sqlite3_operation.h"
#include <opencv2/core/hal/interface.h>
#include <iostream>
#include <fstream>

sqlite3 *db = NULL;
char zErrMsg = 0;
int rc, id;
sqlite3_stmt *stmt;

/*!
 * \fn     Connection_sqlite3DataBase
 * \brief  �������ݿ�
 *
 *
 * \retval int
 */
int Connection_sqlite3DataBase()
{
  rc = sqlite3_open("/userdata/face.db", &db);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
  else
  {
    printf("You have opened a sqlite3 database named bind.db successfully!\nCongratulation! Have fun!\n");
  }
  return 0;
}

/*!
 * \fn     insert_face_data_toDataBase
 * \brief  �������ݿ�
 *
 * \param  [in] const char * name       #
 * \param  [in] float feature [ 512 ]   #
 * \param  [in] int featureSize         #
 *
 * \retval void
 */
void insert_face_data_toDataBase(const char *name, float feature[512], int featureSize, uint8_t *image_data, int image_size)
{
  printf("face_size = %d\n", image_size);
  sqlite3_prepare(db, "insert into face_data_table(name,face_feature,feature_size,image_data,image_size) values (?,?,?,?,?);", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, name, strlen(name), NULL);
  sqlite3_bind_blob(stmt, 2, feature, featureSize, NULL);
  sqlite3_bind_int(stmt, 3, featureSize);
  sqlite3_bind_blob(stmt, 4, image_data, image_size, NULL);
  sqlite3_bind_int(stmt, 5, image_size);

  printf("insert face feature: ");
  for (int i = 0; i < 50; i++)
  {
    printf("%f ", feature[i]);
  }
  printf("\n");
  sqlite3_step(stmt);
}

/*!
 * \fn     QueryFaceFeature
 * \brief  ��ѯ��������
 *
 *
 * \retval map<string, rockx_face_feature_t>
 */
map<string, rockx_face_feature_t> QueryFaceFeature()
{
  rockx_face_feature_t rockx_face_feature = {0, 0};
  map<string, rockx_face_feature_t> rockx_face_feature_map;
  sqlite3_stmt *stmt;
  char *sql = "select name, feature_size, face_feature from face_data_table";

  int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
  int id = 0, len = 0;
  char *name;
  int feature_size;

  if (ret == SQLITE_OK)
  {
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      name = (char *)sqlite3_column_text(stmt, 0);
      printf("name = %s\n", name);
      feature_size = sqlite3_column_int(stmt, 1);
      printf("feature_size = %d\n", feature_size);

      const void *feature = sqlite3_column_blob(stmt, 2);
      memset(rockx_face_feature.feature, 0, feature_size);
      memcpy(rockx_face_feature.feature, feature, feature_size);

      printf("feature: ");
      for (int i = 0; i < 50; i++)
      {
        printf("%f ", rockx_face_feature.feature[i]);
      }
      printf("\n");

      rockx_face_feature.len = feature_size;
      string str(name);
      rockx_face_feature_map.insert(pair<string, rockx_face_feature_t>(str, rockx_face_feature));
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return rockx_face_feature_map;
}

map<People, rockx_face_feature_t> QueryPeopleData()
{
  rockx_face_feature_t rockx_face_feature = {0, 0};
  map<People, rockx_face_feature_t> people_map;
  sqlite3_stmt *stmt;
  char *sql = "select name, feature_size, face_feature, image_size, image_data from face_data_table";

  int id = 0, len = 0;
  char *name;
  int feature_size;
  int image_size;
  vector<char> images;
  People first_people;

  int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
  if (ret == SQLITE_OK)
  {
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      name = (char *)sqlite3_column_text(stmt, 0);
      printf("name = %s\n", name);
      feature_size = sqlite3_column_int(stmt, 1);
      printf("feature_size = %d\n", feature_size);

      const void *feature = sqlite3_column_blob(stmt, 2);
      memset(rockx_face_feature.feature, 0, feature_size);
      memcpy(rockx_face_feature.feature, feature, feature_size);
      rockx_face_feature.len = feature_size;

      image_size = sqlite3_column_int(stmt, 3);
      printf("image_size = %d\n", image_size);

      const char * image_data = (const char *)sqlite3_column_blob(stmt, 4);
      for (int i = 0; i < image_size; i++)
      {
         images.push_back(image_data[i]);
      }

      first_people.people_name = string(name);
      first_people.images = images;

      string str(name);
      //people_map.insert(pair<const People, rockx_face_feature_t>(first_people, rockx_face_feature));
      people_map.insert(make_pair(first_people, rockx_face_feature));
      //people_map.insert(first_people, rockx_face_feature);
    }
  }

  return people_map;
}

#if 0
vector<char> QueryImageData()
{
  vector<char> images;
  sqlite3_stmt *stmt;
  char *sql = "select image_data, image_size from face_data_table";

  int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
  int id = 0, len = 0;
  char *name;
  int feature_size;
  const char *feature;
  FILE *png_file = fopen("image_output.jpg", "w+");

  if (ret == SQLITE_OK)
  {
    printf("ssdsdasda\n");
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      printf("ssssss\n");
      feature = (const char *)sqlite3_column_blob(stmt, 0);
      feature_size = sqlite3_column_int(stmt, 1);
      // printf("feature_size = %d\n", feature_size);
      // fwrite(feature, sizeof(char), 18064, png_file);
      for (int i = 0; i < feature_size; i++)
      {
        images.push_back(feature[i]);
      }

      // images(feature, feature + 18064);
      //  memset(rockx_face_feature.feature, 0, feature_size);
      //  memcpy(rockx_face_feature.feature, feature, feature_size);
    }
  }

  return images;
}
#endif

struct People QueryImageData()
{
  struct People people;
  vector<char> images;
  sqlite3_stmt *stmt;
  char *sql = "select name, image_data, image_size from face_data_table";

  int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
  int id = 0, len = 0;
  char *name;
  int feature_size;
  const char *feature;

  if (ret == SQLITE_OK)
  {
    printf("ssdsdasda\n");
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {

      printf("ssssss\n");
      name = (char *)sqlite3_column_text(stmt, 0);
      feature = (const char *)sqlite3_column_blob(stmt, 1);
      feature_size = sqlite3_column_int(stmt, 2);
      for (int i = 0; i < feature_size; i++)
      {
        images.push_back(feature[i]);
      }

      string name_str(name);
      people.people_name = name_str;
      people.images = images;
    }
  }

  return people;
}
