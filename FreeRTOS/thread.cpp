#include "thread.hpp"

namespace esf
{
Thread::Thread(void (*func)(void *), const char *name, uint16_t stack_size, void *params, UBaseType_t priority)
{
    xTaskCreate(func, name, stack_size * sizeof(StackType_t), params, priority, &m_handle);
    // vTaskSuspend(m_handle);
}
Thread::Thread(TaskHandle_t handle)
    : m_handle(handle)
{
}
Thread::~Thread()
{
    // vTaskDelete(m_handle);
}

Thread &Thread::join()
{
    if (eTaskGetState(m_handle) == eSuspended) {
        vTaskResume(m_handle);
    }
    return *this;
}

Thread &Thread::suspend()
{
    if (eTaskGetState(m_handle) != eSuspended) {
        vTaskSuspend(NULL);
    }
    return *this;
}

Thread &Thread::sleep(size_t ms)
{
    osDelay(ms);
    return *this;
}

Thread &Thread::sleep_for(size_t ms)
{
    osDelayUntil(ms);
    return *this;
}

void Thread::join(Thread &thread)
{
    thread.join();
}

void Thread::suspend(Thread &thread)
{
    if (eTaskGetState(thread.m_handle) != eSuspended) {
        vTaskSuspend(thread.m_handle);
    }
}

Thread Thread::this_thread()
{
    auto handle = xTaskGetCurrentTaskHandle();
    return Thread(handle);
}

} // namespace esf