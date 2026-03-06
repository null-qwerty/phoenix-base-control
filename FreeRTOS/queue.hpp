#pragma once

#include "core/errcode.hpp"

#include "FreeRTOS.h"
#include "queue.h"

namespace esf
{
/**
 * @brief 基本队列，封装自 FreeRTOS 的队列，用于不同人物之间的通信
 *
 * @tparam T 队列元素类型
 */
template <typename T>
class Queue {
public:
    /**
     * @brief 构造函数
     *
     * @param length 队列长度
     */
    Queue(size_t length);
    /**
     * @brief 析构函数
     *
     */
    ~Queue();

    /**
     * @brief 向队列尾添加元素
     *
     * @param item 需要添加的元素
     * @param ticksToWait 超时时间，默认 0
     * @return EsfReturnType 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfReturnType push(const T &item, TickType_t ticksToWait = 0);
    /**
     * @brief 从队列头弹出元素
     *
     * @param item 弹出元素存放的位置
     * @param ticksToWait 超时时间，默认 0
     * @return EsfReturnType 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfReturnType pop(T &item, TickType_t ticksToWait = 0);

    /**
     * @brief 从中断向队列尾添加元素
     *
     * @param item 需要添加的元素
     * @param pxHigherPriorityTaskWoken 退出后是否调用高优先级任务
     * @return EsfReturnType 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfReturnType pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken = nullptr);
    EsfReturnType popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken = nullptr);

private:
    QueueHandle_t m_queueHandle; // 队列 handle
};

/**
 * @brief 具有读队列数据结构的基类
 *
 * @tparam elementT 元素类型
 * @tparam QueueLength 队列长度
 *
 * @note 作为 feature 基类
 */
template <typename elementT, int QueueLength = 5>
struct ReadQueueBase {
    ReadQueueBase(size_t length = QueueLength)
        : read_queue(length)
    {
    }

    Queue<elementT> read_queue;
};
/**
 * @brief 具有写队列数据结构的基类
 *
 * @tparam elementT 元素类型
 * @tparam QueueLength 队列长度
 *
 * @note 作为 feature 基类
 */
template <typename elementT, int QueueLength = 5>
struct WriteQueueBase {
    WriteQueueBase(size_t length = QueueLength)
        : write_queue(length)
    {
    }

    Queue<elementT> write_queue;
};
} // namespace esf