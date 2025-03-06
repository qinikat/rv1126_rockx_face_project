#include "rv1126_isp_function.h"

int init_all_isp_function()
{
    rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
    SAMPLE_COMM_ISP_Init(hdr_mode, RK_FALSE);
    SAMPLE_COMM_ISP_Run();
    SAMPLE_COMM_ISP_SetFrameRate(30);

    return 0;
}