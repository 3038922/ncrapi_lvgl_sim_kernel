#pragma once

#define USE_PROS_LVGL_SIM 1 //0 If you are compiling a standard vex program \
                            //1 If you want to use a simulator

#if USE_PROS_LVGL_SIM == 1
#define LVGL_INCLUDE_PATH "../ncrapi_lvgl_sim_kernel/lvgl/lvgl.h"
#else
#define LVGL_INCLUDE_PATH "display/lvgl.h"
#endif
