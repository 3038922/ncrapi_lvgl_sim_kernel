#pragma once
#include "../ncr_lvgl_sim_conf.h"
#if USE_PROS_LVGL_SIM == 1
#define SDL_MAIN_HANDLED //这个似乎WIN下一定要加
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mouse.h "
#include <array>
#include <conio.h>
#include <thread>
#define PROS_ERR (INT32_MAX)
#define PROS_ERR_F (INFINITY)
namespace ncrapi {
enum {
    INIT,
    AUTONOMOUS,
    OPCONTROL,
    COMP,
    DISABLE
} FUN_NAME;

class NcrLvglSimKernel
{
  public:
    //饿汉模式单例实现.线程安全
    static NcrLvglSimKernel *initNcrLvglSimKernel(void (*f1)(), void (*f2)(), void (*f3)(), void (*f4)(), void (*f5)());
    //0 leftX 1 leftY 2 rightX 3 rightY
    static int GetSimCh(int);
    static int GetSimDig(int);
    static bool isSTop;
    void mainLoop();
    void motorSimLoop(const int tickTime);

  private:
    NcrLvglSimKernel(void (*f1)(), void (*f2)(), void (*f3)(), void (*f4)(), void (*f5)());
    ~NcrLvglSimKernel();
    static void cleanTask(std::thread *task);
    static NcrLvglSimKernel *_ncrLvglSimKernel; // 单例对象
    std::thread *_lvglTask = nullptr;
};
} // namespace ncrapi
extern "C" {
LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
}
#else
#endif