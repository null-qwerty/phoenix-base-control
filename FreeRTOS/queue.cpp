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
EsfReturnType Queue<T>::push(const T &item, TickType_t ticksToWait)
{
    auto err = xQueueSend(m_queueHandle, &item, ticksToWait);
    return err == pdPASS ? ESF_SUCCESS : (err == errQUEUE_FULL ? ESF_QUEUE_FULL : ESF_TIMEOUT);
}

template <typename T>
EsfReturnType Queue<T>::pop(T &item, TickType_t ticksToWait)
{
    auto err = xQueueReceive(m_queueHandle, &item, ticksToWait);
    return err == pdPASS ? ESF_SUCCESS : (err == errQUEUE_EMPTY ? ESF_QUEUE_EMPTY : ESF_TIMEOUT);
}

template <typename T>
EsfReturnType Queue<T>::pushFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken == nullptr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        auto err = xQueueSendFromISR(m_queueHandle, &item, &xHigherPriorityTaskWoken);
        return err == pdPASS ? ESF_SUCCESS : ESF_TIMEOUT;
    }
    auto err = xQueueSendFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken);
    return err == pdPASS ? ESF_SUCCESS : ESF_TIMEOUT;
}

template <typename T>
EsfReturnType Queue<T>::popFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken == nullptr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        auto err = xQueueReceiveFromISR(m_queueHandle, &item, &xHigherPriorityTaskWoken);
        return err == pdPASS ? ESF_SUCCESS : ESF_TIMEOUT;
    }
    auto err = xQueueReceiveFromISR(m_queueHandle, &item, pxHigherPriorityTaskWoken);
    return err == pdPASS ? ESF_SUCCESS : ESF_TIMEOUT;
}

} // namespace esf