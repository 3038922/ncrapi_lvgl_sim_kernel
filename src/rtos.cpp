
#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "lvgl/lvgl.h"
extern "C" {
#include "../freeRTOS/src/main.h"
}
namespace pros {
namespace c {
uint32_t millis(void)
{
    return xTaskGetTickCount();
}
void delay(const uint32_t milliseconds)
{
    vTaskDelay(milliseconds);
}
} // namespace c
using namespace pros::c;

Task::Task(task_fn_t function, void *parameters, std::uint32_t prio, std::uint16_t stack_depth, const char *name)
{
    xTaskCreate(function, name, 512, parameters, prio, (TaskHandle_t *)(&task));
}

Task::Task(task_fn_t function, void *parameters, const char *name)
    : Task(function, parameters, TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, name) {}

Task::Task(task_t task) : task(task) {}
void Task::operator=(const task_t in)
{
    task = in;
}

Task Task::current()
{
    return Task(xTaskGetCurrentTaskHandle());
}

void Task::remove()
{
    return vTaskDelete((TaskHandle_t)(task));
}

std::uint32_t Task::get_priority(void)
{
    return uxTaskPriorityGet((TaskHandle_t)task);
}

void Task::set_priority(std::uint32_t prio)
{
    vTaskPrioritySet((TaskHandle_t)task, prio);
}

std::uint32_t Task::get_state(void)
{
    return eTaskGetState((TaskHandle_t)task);
}

void Task::suspend(void)
{
    vTaskSuspend((TaskHandle_t)task);
}

void Task::resume(void)
{
    vTaskResume((TaskHandle_t)task);
}

const char *Task::get_name(void)
{
    return pcTaskGetName((TaskHandle_t)task);
}

std::uint32_t Task::notify(void)
{
    return notify_ext((0), E_NOTIFY_ACTION_INCR, NULL);
}

std::uint32_t Task::notify_ext(std::uint32_t value, notify_action_e_t action, std::uint32_t *prev_value)
{
    return xTaskGenericNotify((TaskHandle_t)task, value, (eNotifyAction)action, prev_value);
}

std::uint32_t Task::notify_take(bool clear_on_exit, std::uint32_t timeout)
{
    return ulTaskNotifyTake(clear_on_exit, timeout);
}

bool Task::notify_clear(void)
{
    return xTaskNotifyStateClear((TaskHandle_t)task);
}

void Task::delay(const std::uint32_t milliseconds)
{
    vTaskDelay(milliseconds);
}

void Task::delay_until(std::uint32_t *const prev_time, const std::uint32_t delta)
{
    vTaskDelayUntil(prev_time, delta);
}

std::uint32_t Task::get_count(void)
{
    return uxTaskGetNumberOfTasks();
}

Mutex::Mutex(void)
{
    mutex = (mutex_t)xQueueCreateMutex(queueQUEUE_TYPE_MUTEX);
}

bool Mutex::take(std::uint32_t timeout)
{
    return xQueueSemaphoreTake((QueueHandle_t)mutex, timeout);
}

bool Mutex::give(void)
{
    return xQueueGenericSend((QueueHandle_t)(mutex), NULL, queueQUEUE_TYPE_SET, queueSEND_TO_BACK);
}
} // namespace pros

#else
#endif