#include "connectivity.hpp"

namespace esf
{
namespace base
{
template <typename Derived, typename QueueType>
bool Connectivity<Derived, QueueType>::sendMessage(uint8_t *data, size_t size, uint32_t target_id, MessageType type)
{
    return static_cast<Derived *>(this)->sendMessageImpl(data, size, target_id, type);
}

template <typename Derived, typename QueueType>
bool Connectivity<Derived, QueueType>::receiveMessage(uint8_t *buffer, size_t size, uint32_t source_id,
                                                      MessageType type)
{
    return static_cast<Derived *>(this)->receiveMessageImpl(buffer, size, source_id, type);
}
} // namespace base
} // namespace esf