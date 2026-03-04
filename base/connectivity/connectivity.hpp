#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "queue.hpp"

namespace esf
{
namespace base
{
template <typename Derived, typename QueueType>
class Connectivity : public WithQueueTypeBase<QueueType, 5> {
public:
    enum class MessageType : uint8_t { DATA_STREAM = 0, CAN_STD, CAN_EXT, UART, SPI, I2C, RS485, COUNT };
    virtual bool sendMessage(uint8_t *data, size_t size, uint32_t target_id, MessageType type) final;
    virtual bool receiveMessage(uint8_t *buffer, size_t size, uint32_t source_id, MessageType type) final;

protected:
    Connectivity() = default;
    virtual ~Connectivity() = default;
};

} // namespace base
} // namespace esf