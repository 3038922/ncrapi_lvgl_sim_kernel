
#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "lvgl/lvgl.h"
extern "C" {
#include "freeRTOS/src/userInit/userInit.h"
#include "task.h"
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
// Task &Task::operator=(task_t in)
// {
//     task = in;
//     return this;
// }

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
    return 1;
    // /xTaskGenericNotify((TaskHandle_t)task, value, (eNotifyAction)action, prev_value);
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
extern "C" {
queue_t xQueueCreateMutex(const uint8_t ucQueueType)
{
    Queue_t *pxNewQueue;
    const uint32_t uxMutexLength = (uint32_t)1, uxMutexSize = (uint32_t)0;
    pxNewQueue = (Queue_t *)xQueueGenericCreate(uxMutexLength, uxMutexSize, ucQueueType);
    if (pxNewQueue != NULL)
    {
        /* The queue create function will set all the queue structure members
			correctly for a generic queue, but this function is creating a
			mutex.  Overwrite those members that need to be set differently -
			in particular the information required for priority inheritance. */
        pxNewQueue->u.xSemaphore.xMutexHolder = NULL;
        pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;

        /* In case this is a recursive mutex. */
        pxNewQueue->u.xSemaphore.uxRecursiveCallCount = 0;

        traceCREATE_MUTEX(pxNewQueue);

        /* Start with the semaphore in the expected state. */
        (void)xQueueGenericSend(pxNewQueue, NULL, (TickType_t)0U, queueSEND_TO_BACK);
    }
    else
    {
        traceCREATE_MUTEX_FAILED();
    }
    return pxNewQueue;
}
}
Mutex::Mutex(void) : mutex((mutex_t)xQueueCreateMutex(queueQUEUE_TYPE_MUTEX), traceQUEUE_DELETE) {}

bool Mutex::take(std::uint32_t timeout)
{
    return xQueueSemaphoreTake((mutex), (timeout));
}

bool Mutex::give(void)
{
    return xQueueGenericSend((queue_t)(mutex), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK);
}
} // namespace pros

#else
#endif