#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "queue.hpp"
#include "errcode.hpp"

#include <functional>

namespace esf
{
namespace base
{
template <typename Derived, typename QueueType>
class Connectivity : public ReadQueueBase<QueueType>, public WriteQueueBase<QueueType> {
public:
    enum class MessageType : uint8_t { DATA_STREAM = 0, CAN_STD, CAN_EXT, UART, SPI, I2C, RS485, COUNT };

    virtual EsfReturnType sendMessage(MessageType type) final;
    virtual EsfReturnType receiveMessage(MessageType type) final;

    EsfReturnType pushToSendQueue(QueueType message);
    EsfReturnType popFromReceiveQueue(QueueType &message);
    EsfReturnType setRxCallback(std::function<EsfReturnType(BaseType_t *)> callback);

protected:
    Connectivity() = default;
    virtual ~Connectivity() = default;

    EsfReturnType m_sendErrCode;
    EsfReturnType m_receiveErrCode;

    std::function<EsfReturnType(BaseType_t *)> m_rxCallbackFunc;
};

} // namespace base
} // namespace esf