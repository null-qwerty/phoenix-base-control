#include "queue.hpp"

namespace esf
{
template <typename T>
Queue<T>::Queue(size_t length)
    : m_queueHandle(xQueueCreate(length, sizeof(T)))
{
}

template <typename T>
Queue<T>::~Queue()
{
    if (m_queueHandle) {
        vQueueDelete(m_queueHandle);
    }
}

template <typename T>
bool Queue<T>::push(const T &item, TickType_t ticksToWait)
{
    return xQueueSend(m_queueHandle, &item, ticksToWait) == pdPASS;
}

template <typename T>
bool Queue<T>::pop(T &item, TickType_t ticksToWait)
{
    return xQueueReceive(m_queueHandle, &item, ticksToWait) == pdPASS;
}

template <typename T>
bool Queue<T>::pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken)
{
    return xQueueSendFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken) == pdPASS;
}

template <typename T>
bool Queue<T>::popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken)
{
    return xQueueReceiveFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken) == pdPASS;
}

} // namespace esf