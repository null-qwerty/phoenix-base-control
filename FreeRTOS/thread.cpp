#include "thread.hpp"

namespace esf
{
Thread::Thread(void (*func)(void *), const char *name, uint16_t stack_size,
               void *params, UBaseType_t priority)
    : m_func(func)
{
    xTaskCreate(m_func, name, stack_size * sizeof(StackType_t), params,
                priority, &m_handle);
    vTaskSuspend(m_handle);
}

Thread::~Thread()
{
    vTaskDelete(m_handle);
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
        vTaskSuspend(m_handle);
    }
    return *this;
}

void Thread::join(Thread &thread)
{
    thread.join();
}

void Thread::suspend(Thread &thread)
{
    thread.suspend();
}

} // namespace esf