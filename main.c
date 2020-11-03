

#undef main
#include "./lvgl/lvgl.h"
#include "SDL2/SDL.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_examples/lv_examples.h"
#include "lvgl/lvgl.h"
#include <Windows.h>
#include <stdlib.h>
void memory_monitor(lv_task_t *param)
{
    (void)param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    // printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n",
    //        (int)mon.total_size - mon.free_size, mon.used_pct, mon.frag_pct,
    //        (int)mon.free_biggest_size);
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
static lv_indev_t *kb_indev;

void hal_init(void)
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
    lv_indev_drv_t kb_drv;
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = keyboard_read;
    kb_indev = lv_indev_drv_register(&kb_drv);
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

int main()
{
    lv_init();

    /*Initialize the HAL for LittlevGL*/
    hal_init();
    lv_demo_widgets();
    while (1)
    {
        /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        Sleep(10); /*Just to let the system breathe */
    }
    return 0;
}