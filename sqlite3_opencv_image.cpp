#include "sqlite3_operation.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <vector>
#include <fstream>

using namespace cv;

int main()
{
    Connection_sqlite3DataBase();
    

#if 0
    //vector<char> images = QueryImageData();
    People people = QueryImageData();
    printf("peope_name = %s, image_size = %d\n",people.people_name.c_str(),people.images.size());
    cv::Mat image_mat = cv::imdecode(people.images, CV_LOAD_IMAGE_COLOR);
    if (image_mat.empty())
    {
        printf("failed to decode image\n");
        return -1;
    }

    cv::imwrite("blob_output.jpg", image_mat);
#endif

     //map<People, rockx_face_feature_t> people_map = QueryPeopleData();
#if 0
    ifstream ifs("chenzhi02.png", ios::binary | ios::ate);
    int file_size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    vector<char> buffer(file_size);
    ifs.read(buffer.data(), file_size);
    ifs.close();
    // 解码图像
    Mat img = imdecode(Mat(buffer), IMREAD_COLOR);
    if (img.empty())
    {
        printf("failed to decode image\n");
        return -1;
    }
    imwrite("output_blog.jpg",img);
#endif

#if 0
    vector<char> images = QueryImageData();
    vector<unsigned char> img_encode;
    printf("image_size = %d\n", images.size());
    cv::imencode(".jpg", images, img_encode);

    Mat img = imdecode(Mat(img_encode), IMREAD_COLOR);
    if (img.empty())
    {
        printf("failed to decode image\n");
        return -1;
    }
    imwrite("output_blog_02.jpg",img);
#endif

#if 0
    FILE *imageFile = fopen("chenzhi02.png", "rb"); // 打开名为image.jpg的图片文件（二进制模式）
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
    printf("bytesRead = %d\n", bytesRead);
    fclose(imageFile);   
    const char * name = "harry"; 
    float feature[512] ={};

    insert_face_data_toDataBase(name,feature,512,buffer,bytesRead);
#endif

    return 0;
}
