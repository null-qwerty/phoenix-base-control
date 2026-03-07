#include "connectivity.hpp"

namespace esf
{
namespace base
{
template <typename Derived, typename Message>
EsfStatus Connectivity<Derived, Message>::sendMessage(MessageType type)
{
    return static_cast<Derived *>(this)->sendMessageImpl(type);
}

template <typename Derived, typename Message>
EsfStatus Connectivity<Derived, Message>::receiveMessage(MessageType type)
{
    return static_cast<Derived *>(this)->receiveMessageImpl(type);
}

template <typename Derived, typename Message>
EsfStatus Connectivity<Derived, Message>::pushToSendQueue(Message message)
{
    return this->read_queue.push(message);
}

template <typename Derived, typename Message>
EsfStatus Connectivity<Derived, Message>::popFromReceiveQueue(Message &message)
{
    return this->write_queue.pop(message);
}

template <typename Derived, typename Message>
Connectivity<Derived, Message> Connectivity<Derived, Message>::setReceiveDataSize(size_t size)
{
    m_receiveData.write().size = size;
    return *this;
}

template <typename Derived, typename Message>
EsfStatus Connectivity<Derived, Message>::setRxCallback(std::function<EsfStatus(BaseType_t *)> &callback)
{
    m_rxCallbackFunc = callback;
    return ESF_SUCCESS;
}
} // namespace base
} // namespace esf