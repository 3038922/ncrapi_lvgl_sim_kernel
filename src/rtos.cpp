
#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "lvgl/lvgl.h"
#include "pros/rtos.hpp"
#include <ctime>
#include <thread>

namespace pros {

namespace c {
uint32_t millis(void)
{
    return lv_tick_get();
}
void delay(const uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
} // namespace c
Task::Task(task_fn_t function, void *parameters, std::uint32_t prio, std::uint16_t stack_depth, const char *name)
{
    task = new std::thread(function, parameters);
}
void Task::remove()
{
    ((std::thread *)(task))->detach();
    delete ((std::thread *)(task));
    task = nullptr;
}

using std::chrono::system_clock;
void Task::delay_until(std::uint32_t *const prev_time, const std::uint32_t delta)
{
    // std::time_t tt = system_clock::to_time_t(system_clock::now());
    // struct std::tm *ptm = std::localtime(&tt);
    // ++ptm->tm_min;
    // ptm->tm_sec = 0;
    // std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
    std::this_thread::sleep_for(std::chrono::milliseconds(delta));
}
std::uint32_t Task::get_state()
{
    if (task == nullptr)
        return 0;
    else
        return 4;
}
} // namespace pros
#else
#endif