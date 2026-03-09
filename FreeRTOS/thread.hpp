#pragma once

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "portmacro.h"
#include "task.h"
#include <cstddef>

namespace esf
{
class Thread {
public:
    /**
     * @brief 创建线程
     *
     * @param func 线程函数
     * @param name 线程名称
     * @param stack_size 栈大小，最终分配的大小为 stack_size * 4 字节，默认值为
     * 128
     * @param params 线程参数，默认为 NULL
     * @param priority 线程优先级，默认值为 osPriorityNormal
     *
     * @note 线程创建后处于挂起状态，需调用 join() 启动线程
     */
    Thread(void (*func)(void *), const char *name, uint16_t stack_size = 128, void *params = NULL,
           UBaseType_t priority = osPriorityNormal);
    /**
     * @brief 从 task 句柄创建线程
     *
     * @param handle 句柄
     */
    Thread(TaskHandle_t handle);
    /**
     * @brief 销毁线程
     *
     */
    ~Thread();
    /**
     * @brief 启动线程
     *
     */
    Thread &join();
    /**
     * @brief 挂起线程
     *
     */
    Thread &suspend();
    /**
     * @brief 阻塞线程
     *
     * @param ms 阻塞的时间，绝对延时
     * @return Thread& 自身引用
     */
    Thread &sleep(size_t ms);
    /**
     * @brief 阻塞线程
     *
     * @param ms 阻塞的时间，相对延时
     * @return Thread& 自身引用
     */
    Thread &sleep_for(size_t ms);

    /**
     * @brief 启动一个线程
     *
     * @param thread 需要启动的线程
     */
    static void join(Thread &thread);
    /**
     * @brief 挂起一个线程
     *
     * @param thread 需要挂起的线程
     */
    static void suspend(Thread &thread);

    /**
     * @brief 获取当前线程
     *
     * @return Thread 当前线程对象
     */
    static Thread this_thread();

private:
    TaskHandle_t m_handle;
};
} // namespace esf