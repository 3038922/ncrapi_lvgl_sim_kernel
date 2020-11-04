
#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "../lvgl/lvgl.h"
#include "keyBoardDrive.hpp"
#include "lv_examples/lv_examples.h"
#include "pros/misc.h"
#include "pros/rtos.hpp"
#include <algorithm>
#include <array>
#include <iostream>

extern "C" {
#include "../freeRTOS/src/main.h"
}
/**
 * Print the memory usage periodically
 * @param param
 */
void memory_monitor(lv_task_t *param)
{
    (void)param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n",
           (int)mon.total_size - mon.free_size, mon.used_pct, mon.frag_pct,
           (int)mon.free_biggest_size);
}
/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data)
{
    (void)data;

    while (1)
    {
        lv_tick_inc(5);
        SDL_Delay(5); /*Sleep for 1 millisecond*/
    }
    return 0;
}

static void hal_init(void)
{
    /* Add a display
   * Use the 'monitor' driver which creates window on PC's monitor to simulate a
   * display*/
    monitor_init();

    static lv_disp_buf_t disp_buf1;
    static lv_color_t buf1_1[LV_HOR_RES_MAX * 120];
    lv_disp_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX * 120);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/
    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;
    lv_disp_drv_register(&disp_drv);

    /* Add the mouse (or touchpad) as input device
   * Use the 'mouse' driver which reads the PC's mouse*/
    mouse_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb =
        mouse_read; /*This function will be called periodically (by the library)
                     to get the mouse position and state*/
    lv_indev_drv_register(&indev_drv);

    /* If the PC keyboard driver is enabled in`lv_drv_conf.h`
   * add this as an input device. It might be used in some examples. */
#if USE_KEYBOARD
    // lv_indev_drv_t kb_drv;
    // lv_indev_drv_init(&kb_drv);
    // kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    // kb_drv.read_cb = keyboard_read;
    // kb_indev = lv_indev_drv_register(&kb_drv);
#endif

    /* Tick init.
   * You have to call 'lv_tick_inc()' in every milliseconds
   * Create an SDL thread to do this*/
    SDL_CreateThread(tick_thread, "tick", NULL);

    /* Optional:
   * Create a memory monitor task which prints the memory usage in
   * periodically.*/
    // lv_task_create(memory_monitor, 5000, LV_TASK_PRIO_MID, NULL);
}
void taskLVGL(void *pragma)
{
    (void)pragma;
    while (1)
    {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        pros::delay(10);
    }
    vTaskDelete(nullptr);
}
extern "C" {
extern void autonomous(void);
extern void initialize(void);
extern void disabled(void);
extern void competition_initialize(void);
extern void opcontrol(void);
}

namespace ncrapi {
void taskAutonomous(void *pargma)
{
    autonomous();
}
void taskDisabled(void *pargma)
{
    disabled();
}
void taskCompetition_initialize(void *pargma)
{
    competition_initialize();
}
void taskOpcontrol(void *pargma)
{
    opcontrol();
}
void deleteTask(pros::Task *task)
{
    if (task != nullptr)
    {
        task->remove();
        delete task;
        task = nullptr;
    }
}
void taskMainLoop(void *)
{
    initialize();
    KeyBoard *keyboard = KeyBoard::initKeyBoard();
    int tickTime = 0;
    uint32_t lastTime = 0;
    pros::Task *subTask = new pros::Task((pros::task_fn_t)taskOpcontrol, nullptr, "taskOpcontrol");
    while (1)
    {
        tickTime = pros::millis() - lastTime;
        if (keyboard->getFiledNewPressed(KEYBOARD_DIGITAL_1))
        {
            deleteTask(subTask);
            subTask = new pros::Task((pros::task_fn_t)taskAutonomous, nullptr, "taskAuto");
        }
        else if (keyboard->getFiledNewPressed(KEYBOARD_DIGITAL_2))
        {
            deleteTask(subTask);
            subTask = new pros::Task((pros::task_fn_t)taskOpcontrol, nullptr, "taskOpcontrol");
        }
        else if (keyboard->getFiledNewPressed(KEYBOARD_DIGITAL_3))
        {
            deleteTask(subTask);
            subTask = new pros::Task((pros::task_fn_t)taskCompetition_initialize, nullptr, "taskCompetition");
        }
        else if (keyboard->getFiledNewPressed(KEYBOARD_DIGITAL_4))
        {
            deleteTask(subTask);
            subTask = new pros::Task((pros::task_fn_t)taskDisabled, nullptr, "taskDisabled");
        }
        NcrLvglSimKernel::initNcrLvglSimKernel()->motorSimLoop(tickTime);
        lastTime = pros::millis();
        pros::delay(10);
    }
}

NcrLvglSimKernel *NcrLvglSimKernel::_ncrLvglSimKernel = nullptr; // 单例定义
NcrLvglSimKernel *NcrLvglSimKernel::initNcrLvglSimKernel()
{
    if (_ncrLvglSimKernel == nullptr)
        _ncrLvglSimKernel = new NcrLvglSimKernel();
    return _ncrLvglSimKernel;
}

NcrLvglSimKernel::NcrLvglSimKernel()
{
    /*Initialize LittlevGL*/
    lv_init();
    /*Initialize the HAL (display, input devices, tick) for LittlevGL*/
    hal_init();
    // xTaskCreate(taskLVGL, "lvglTask", 512, nullptr, TASK_PRIORITY_MAX - 3, nullptr);
    // xTaskCreate(taskMainLoop, "mainLoopTask", 512, nullptr, TASK_PRIORITY_MAX - 2, nullptr);
}
NcrLvglSimKernel::~NcrLvglSimKernel()
{
}

} // namespace ncrapi

#else
#endif