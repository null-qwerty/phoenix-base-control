#pragma once

#include "FreeRTOS.h"
#include "queue.h"

namespace esf
{
template <typename T>
class Queue {
public:
    Queue(size_t length);
    ~Queue();

    bool push(const T &item, TickType_t ticksToWait = 0);
    bool pop(T &item, TickType_t ticksToWait = 0);

    bool pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken);
    bool popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken);

private:
    QueueHandle_t m_queueHandle;
};

template <typename elementT, int QueueLength = 5>
struct WithQueueTypeBase {
    WithQueueTypeBase(size_t length = QueueLength)
        : queue(length)
    {
    }

    Queue<elementT> queue;
};
} // namespace esf