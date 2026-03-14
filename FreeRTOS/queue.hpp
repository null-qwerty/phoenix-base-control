#pragma once

#include "core/status.hpp"

#include "FreeRTOS.h"
#include "queue.h"

namespace esf
{
/**
 * @brief 基本队列，封装自 FreeRTOS 的队列，用于不同人物之间的通信
 *
 * @tparam T 队列元素类型
 */
template <typename T> class Queue {
public:
    /**
     * @brief 构造函数
     *
     * @param length 队列长度
     */
    Queue(size_t length)
        : m_queueHandle(xQueueCreate(length, sizeof(T)))
    {
    }
    /**
     * @brief 析构函数
     *
     */
    ~Queue()
    {
        if (m_queueHandle) {
            vQueueDelete(m_queueHandle);
        }
    }

    /**
     * @brief 向队列尾添加元素
     *
     * @param item 需要添加的元素
     * @param ticksToWait 超时时间，默认 0
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus push(const T &item, TickType_t ticksToWait = 0)
    {
        auto err = xQueueSend(m_queueHandle, &item, ticksToWait);
        return err == pdPASS ? ESF_SUCCESS : (err == errQUEUE_FULL ? ESF_QUEUE_FULL : ESF_TIMEOUT);
    }
    /**
     * @brief 从队列头弹出元素
     *
     * @param item 弹出元素存放的位置
     * @param ticksToWait 超时时间，默认 0
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus pop(T &item, TickType_t ticksToWait = 0)
    {
        auto err = xQueueReceive(m_queueHandle, &item, ticksToWait);
        return err == pdPASS ? ESF_SUCCESS
                             : (err == errQUEUE_EMPTY ? ESF_QUEUE_EMPTY : ESF_TIMEOUT);
    }

    /**
     * @brief 从中断向队列尾添加元素
     *
     * @param item 需要添加的元素
     * @param pxHigherPriorityTaskWoken 退出后是否调用高优先级任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken = nullptr)
    {
        if (pxHigherPriorityTaskWoken == nullptr) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            auto err = xQueueSendFromISR(m_queueHandle, &item, &xHigherPriorityTaskWoken);
            return err == pdPASS ? ESF_SUCCESS : ESF_QUEUE_PUSH_ERROR;
        }
        auto err = xQueueSendFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken);
        return err == pdPASS ? ESF_SUCCESS : ESF_QUEUE_PUSH_ERROR;
    }
    /**
     * @brief 从队列头弹出元素到中断
     *
     * @param item 弹出元素存放的位置
     * @param pxHigherPriorityTaskWoken 退出后是否调用高优先级任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken = nullptr)
    {
        if (pxHigherPriorityTaskWoken == nullptr) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            auto err = xQueueReceiveFromISR(m_queueHandle, &item, &xHigherPriorityTaskWoken);
            return err == pdPASS ? ESF_SUCCESS : ESF_QUEUE_POP_ERROR;
        }
        auto err = xQueueReceiveFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken);
        return err == pdPASS ? ESF_SUCCESS : ESF_QUEUE_POP_ERROR;
    }

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
template <typename elementT, int QueueLength = 5> struct ReadQueueBase {
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
template <typename elementT, int QueueLength = 5> struct WriteQueueBase {
    WriteQueueBase(size_t length = QueueLength)
        : write_queue(length)
    {
    }

    Queue<elementT> write_queue;
};
} // namespace esf