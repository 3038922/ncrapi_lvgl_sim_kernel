#pragma once
#include "../ncr_lvgl_sim_conf.h"
#if USE_PROS_LVGL_SIM == 1

#define SDL_MAIN_HANDLED //这个似乎WIN下一定要加
#include "../lv_drivers/display/monitor.h"
#include "../lv_drivers/indev/keyboard.h"
#include "../lv_drivers/indev/mouse.h "
#include "pros/rtos.hpp"
#include <array>
#include <conio.h>
#define PROS_ERR (INT32_MAX)
#define PROS_ERR_F (INFINITY)
namespace ncrapi {

class NcrLvglSimKernel
{
  public:
    //饿汉模式单例实现.线程安全
    static NcrLvglSimKernel *initNcrLvglSimKernel(); //0 leftX 1 leftY 2 rightX 3 rightY
    void motorSimLoop(const int tickTime);

  private:
    NcrLvglSimKernel();
    ~NcrLvglSimKernel();

    static NcrLvglSimKernel *_ncrLvglSimKernel; // 单例对象
};
} // namespace ncrapi
#else
#endif