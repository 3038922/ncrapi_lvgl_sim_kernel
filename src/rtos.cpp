
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

queue_t xQueueCreateMutex(const uint8_t ucQueueType)
{
    Queue_t *pxNewQueue;
    const uint32_t uxMutexLength = (uint32_t)1, uxMutexSize = (uint32_t)0;

    pxNewQueue = (Queue_t *)xQueueGenericCreate(uxMutexLength, uxMutexSize, ucQueueType);
    prvInitialiseMutex(pxNewQueue);

    return pxNewQueue;
}
void queue_delete(queue_t queue)
{
    Queue_t *const pxQueue = (Queue_t *)queue;

    configASSERT(pxQueue);
    traceQUEUE_DELETE(pxQueue);

#if (configQUEUE_REGISTRY_SIZE > 0)
    {
        vQueueUnregisterQueue(pxQueue);
    }
#endif

#if ((configSUPPORT_DYNAMIC_ALLOCATION == 1) && (configSUPPORT_STATIC_ALLOCATION == 0))
    {
        /* The queue can only have been allocated dynamically - free it
		again. */
        kfree(pxQueue);
    }
#elif ((configSUPPORT_DYNAMIC_ALLOCATION == 1) && (configSUPPORT_STATIC_ALLOCATION == 1))
    {
        /* The queue could have been allocated statically or dynamically, so
		check before attempting to free the memory. */
        if (pxQueue->ucStaticallyAllocated == (uint8_t)pdFALSE)
        {
            kfree(pxQueue);
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
#else
    {
        /* The queue must have been statically allocated, so is not going to be
		deleted.  Avoid compiler warnings about the unused parameter. */
        (void)pxQueue;
    }
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
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