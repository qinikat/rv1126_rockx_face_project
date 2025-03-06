// Copyright 2019 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __RKMEDIA_VI_
#define __RKMEDIA_VI_
#ifdef __cplusplus
extern "C" {
#endif
#include "rkmedia_common.h"

typedef enum rkVI_CHN_WORK_MODE {
  VI_WORK_MODE_NORMAL = 0,
  // for vi single caculate luma.
  // In this mode, vi has no output,
  // and data cannot be obtained from vi.
  VI_WORK_MODE_LUMA_ONLY,
  VI_WORK_MODE_BUTT
} VI_CHN_WORK_MODE;

typedef enum rkVI_CHN_BUF_TYPE {
  VI_CHN_BUF_TYPE_DMA = 0, // Default
  VI_CHN_BUF_TYPE_MMAP,
} VI_CHN_BUF_TYPE;

typedef struct rkVI_CHN_ATTR_S {
  const RK_CHAR *pcVideoNode; //VIDEO视频节点路径
  RK_U32 u32Width;  //视频输入的宽度，一般和CMOS摄像头或者外设的宽度一致
  RK_U32 u32Height; //视频输入的高度，一般和CMOS摄像头或者外设的高度一致
  IMAGE_TYPE_E enPixFmt; //视频输入的图像格式，默认是NV12(IMAGE_TYPE_NV12)
  RK_U32 u32BufCnt;          // VI capture video buffer cnt.
  VI_CHN_BUF_TYPE enBufType; // VI capture video buffer type.
  VI_CHN_WORK_MODE enWorkMode;
} VI_CHN_ATTR_S;

typedef struct rkVIDEO_REGION_INFO_S {
  RK_U32 u32RegionNum; /* count of the region */
  RECT_S *pstRegion;   /* region attribute */
} VIDEO_REGION_INFO_S;

typedef struct rkVI_USERPIC_ATTR_S {
  IMAGE_TYPE_E enPixFmt;
  RK_U32 u32Width;
  RK_U32 u32Height;
  RK_VOID *pvPicPtr;
} VI_USERPIC_ATTR_S;

#ifdef __cplusplus
}
#endif
#endif // #ifndef __RKMEDIA_VI_