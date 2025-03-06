/*
 * ./sqlite3_operation_test name image_path
 */

#include "rockx.h"
#include "sqlite3_operation.h"

#define FEATURE_SIZE 512

rockx_handle_t face_det_handle;
rockx_handle_t face_5landmarks_handle;
rockx_handle_t face_recognize_handle;

rockx_object_t *get_max_face(rockx_object_array_t *face_array)
{
  if (face_array->count == 0)
  {
    return NULL;
  }
  rockx_object_t *max_face = NULL;
  int i;
  for (i = 0; i < face_array->count; i++)
  {
    rockx_object_t *cur_face = &(face_array->object[i]);
    if (max_face == NULL)
    {
      max_face = cur_face;
      continue;
    }
    int cur_face_box_area = (cur_face->box.right - cur_face->box.left) *
                            (cur_face->box.bottom - cur_face->box.top);
    int max_face_box_area = (max_face->box.right - max_face->box.left) *
                            (max_face->box.bottom - max_face->box.top);
    if (cur_face_box_area > max_face_box_area)
    {
      max_face = cur_face;
    }
  }
  printf("get_max_face %d\n", i - 1);

  return max_face;
}

int run_face_recognize(const char *name, rockx_image_t *in_image, rockx_face_feature_t *out_feature, unsigned char * buffer, int buffer_size)
{
  rockx_ret_t ret;

  /*************** FACE Detect ***************/
  // create rockx_face_array_t for store result
  rockx_object_array_t face_array;
  memset(&face_array, 0, sizeof(rockx_object_array_t));

  // detect face
  ret = rockx_face_detect(face_det_handle, in_image, &face_array, NULL);
  if (ret != ROCKX_RET_SUCCESS)
  {
    printf("rockx_face_detect error %d\n", ret);
    return -1;
  }

#if 0
  // process result
  for (int i = 0; i < face_array.count; i++)
  {
    int left = face_array.object[i].box.left;
    int top = face_array.object[i].box.top;
    int right = face_array.object[i].box.right;
    int bottom = face_array.object[i].box.bottom;
    float score = face_array.object[i].score;
    printf("%d box=(%d %d %d %d) score=%f\n", i, left, top, right, bottom,
           score);
  }
#endif

  rockx_object_t *max_face = get_max_face(&face_array);
  if (max_face == NULL)
  {
    printf("error no face detected\n");
    return -1;
  }

  // Face Align
  rockx_image_t out_img;
  memset(&out_img, 0, sizeof(rockx_image_t));
  ret = rockx_face_align(face_5landmarks_handle, in_image, &(max_face->box), NULL, &out_img);
  if (ret != ROCKX_RET_SUCCESS)
  {
    return -1;
  }

  // Face Recognition
  rockx_face_recognize(face_recognize_handle, &out_img, out_feature);

  insert_face_data_toDataBase(name, out_feature->feature, FEATURE_SIZE, buffer, buffer_size);

  // Release Aligned Image
  rockx_image_release(&out_img);

  return 0;
}

int main(int argc, char *argv[])
{
  rockx_ret_t ret;

  printf("----------------- main init ---------------------\n");
#if 1
  if (argc != 3)
  {
    printf("Usage: Insert DataBase  ./sqlite3_operation_test name image_path\n");
    return -1;
  }

  printf("Start Connection sqlite3......................\n");
  Connection_sqlite3DataBase();
  printf("End Connection_sqlite3DataBase......................\n");

  rockx_config_t *config = rockx_create_config();
  rockx_add_config(config, ROCKX_CONFIG_DATA_PATH, "/userdata/rockx_data/");

  /*************** Creat Handle ***************/
  // create a face detection handle
  ret = rockx_create(&face_det_handle, ROCKX_MODULE_FACE_DETECTION_V2, config, 0);
  if (ret != ROCKX_RET_SUCCESS)
  {
    printf("init rockx module ROCKX_MODULE_FACE_DETECTION error %d\n", ret);
    return -1;
  }

  // create a face landmark handle
  ret = rockx_create(&face_5landmarks_handle, ROCKX_MODULE_FACE_LANDMARK_5,
                     config, 0);
  if (ret != ROCKX_RET_SUCCESS)
  {
    printf("init rockx module ROCKX_MODULE_FACE_LANDMARK_68 error %d\n", ret);
    return -1;
  }

  // create a face recognize handle
  ret = rockx_create(&face_recognize_handle, ROCKX_MODULE_FACE_RECOGNIZE,
                     config, 0);
  if (ret != ROCKX_RET_SUCCESS)
  {
    printf("init rockx module ROCKX_MODULE_FACE_RECOGNIZE error %d\n", ret);
    return -1;
  }

  const char *name = argv[1];
  const char *image_path = argv[2];

  rockx_face_feature_t out_feature1;
  rockx_image_t input_image1;
  rockx_image_read(image_path, &input_image1, 1);

  FILE *imageFile = fopen(image_path, "rb"); // 打开名为image.jpg的图片文件（二进制模式）
  if (!imageFile)
  {
    printf("无法打开图片文件.\n");
    return -1;
  }

  fseek(imageFile, 0L, SEEK_END);   // 定位到文件尾部获取文件大小
  long fileSize = ftell(imageFile); // 获取文件大小
  rewind(imageFile);                // 重新定位到文件起始位置
  printf("fileSize = %ld\n", fileSize);

  unsigned char *buffer = new unsigned char[fileSize];                          // 分配足够大小的缓冲区
  size_t bytesRead = fread(buffer, sizeof(unsigned char), fileSize, imageFile); // 从文件中读取图像数据到缓冲区

  run_face_recognize(name, &input_image1, &out_feature1, buffer, bytesRead);
#endif
  return 0;
}
