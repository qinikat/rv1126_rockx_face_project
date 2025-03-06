hide := @
ECHO := echo

G++ := /opt/rv1126_rv1109_linux_sdk_v1.8.0_20210224/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
CFLAGS := -I./include/rkmedia \
			-I./include/rkaiq/common \
			-I./include/rkaiq/xcore \
			-I./include/rkaiq/uAPI \
			-I./include/rkaiq/algos \
			-I./include/rkaiq/iq_parser \
			-I./rknn_rockx_include \
			-I./im2d_api          \
			-I./arm_libx264/include \
			-I./arm32_ffmpeg_srt/include \
		        -I./arm_sdl_ttf_install/include/SDL \
                        -I./arm_sdl/include/SDL	\
                        -I./arm_opencv_source/include \
                        -I./sqlite3_source/include \
		        -I./mqtt_install/include \
			-I./cJSON	

LIB_FILES := -L./rv1126_lib -L./opt/arm_libx264/lib -L./opt/arm32_ffmpeg_srt/lib -L./opt/arm_libsrt/lib -L./opt/arm_openssl/lib -L./arm_sdl/lib -L./arm_freetype/lib -L./arm_sdl_ttf_install/lib -L./arm_opencv_source/lib -L./sqlite3_source/lib -L./mqtt_install/lib

LD_FLAGS := -lpthread -leasymedia -ldrm -lrockchip_mpp \
	        -lavformat -lavcodec -lswresample -lavutil \
			-lasound -lv4l2 -lv4lconvert -lrga \
			-lRKAP_ANR -lRKAP_Common -lRKAP_3A \
			-lmd_share -lrkaiq -lod_share  \
			-lx264 -lsrt -lssl -lcrypto -lSDL_ttf -lSDL -lfreetype \
		-lopencv_core -lopencv_dnn \
                        -lopencv_features2d -lopencv_flann -lopencv_highgui \
                        -lopencv_imgcodecs -lopencv_imgcodecs -lopencv_imgproc \
                        -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape \
                        -lopencv_stitching -lopencv_superres -lopencv_videoio -lopencv_video \
                        -lopencv_calib3d -lrockx -lrknn_api -lsqlite3 #-lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lpaho-mqtt3cs
	
			
CFLAGS += -DRKAIQ

all:
	$(G++) ffmpeg_audio_queue.cpp ffmpeg_video_queue.cpp map_manage.cpp sqlite3_operation.cpp rkmedia_assignment_manage.cpp rkmedia_container.cpp rkmedia_data_process.cpp rkmedia_ffmpeg_config.cpp rkmedia_module.cpp rkmedia_module_function.cpp rv1126_ffmpeg_main.cpp rv1126_isp_function.cpp rockx_data_manage.cpp sample_common_isp.c cJSON/cJSON.c  $(CFLAGS) $(LIB_FILES) $(LD_FLAGS) -o rv1126_ffmpeg_main_rockx
	$(G++) sqlite3_operation.cpp sqlite3_operation_test.cpp  $(SAMPLE_COMMON_02) $(CFLAGS) $(LIB_FILES) $(LD_FLAGS) -o sqlite3_operation_test
	$(G++) sqlite3_opencv_image.cpp sqlite3_operation.cpp  $(SAMPLE_COMMON_02) $(CFLAGS) $(LIB_FILES) $(LD_FLAGS) -o sqlite3_opencv_image

	$(hide)$(ECHO) "Build Done ..."

