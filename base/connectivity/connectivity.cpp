#include "connectivity.hpp"

namespace esf
{
namespace base
{
template <typename Derived, typename QueueType>
EsfReturnType Connectivity<Derived, QueueType>::sendMessage(MessageType type)
{
    return static_cast<Derived *>(this)->sendMessageImpl(type);
}

template <typename Derived, typename QueueType>
EsfReturnType Connectivity<Derived, QueueType>::receiveMessage(MessageType type)
{
    return static_cast<Derived *>(this)->receiveMessageImpl(type);
}

template <typename Derived, typename QueueType>
EsfReturnType Connectivity<Derived, QueueType>::pushToSendQueue(QueueType message)
{
    return this->read_queue.push(message);
}

template <typename Derived, typename QueueType>
EsfReturnType Connectivity<Derived, QueueType>::popFromReceiveQueue(QueueType &message)
{
    return this->write_queue.pop(message);
}

template <typename Derived, typename QueueType>
EsfReturnType Connectivity<Derived, QueueType>::setRxCallback(std::function<EsfReturnType(BaseType_t *)> &callback)
{
    m_rxCallbackFunc = callback;
    return ESF_SUCCESS;
}
} // namespace base
} // namespace esf