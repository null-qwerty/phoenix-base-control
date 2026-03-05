#pragma once

#include "core/errcode.hpp"

#include "FreeRTOS.h"
#include "queue.h"

namespace esf
{
template <typename T>
class Queue {
public:
    Queue(size_t length);
    ~Queue();

    EsfReturnType push(const T &item, TickType_t ticksToWait = 0);
    EsfReturnType pop(T &item, TickType_t ticksToWait = 0);

    EsfReturnType pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken);
    EsfReturnType popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken);

private:
    QueueHandle_t m_queueHandle;
};

template <typename elementT, int QueueLength = 5>
struct ReadQueueBase {
    ReadQueueBase(size_t length = QueueLength)
        : read_queue(length)
    {
    }

    Queue<elementT> read_queue;
};

template <typename elementT, int QueueLength = 5>
struct WriteQueueBase {
    WriteQueueBase(size_t length = QueueLength)
        : write_queue(length)
    {
    }

    Queue<elementT> write_queue;
};
} // namespace esf