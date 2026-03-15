#pragma once

#include "connectivity.hpp"
#ifdef HAL_UART_MODULE_ENABLED
#include "portmacro.h"

#include <map>

namespace esf
{
namespace base
{

struct UartMessage {
    uint8_t *data;
    size_t size;
};

class UART : public Connectivity<UART, UartMessage> {
public:
    using Message = UartMessage; // 消息类型定义

    UART(UART_HandleTypeDef &handle, DMA dma);
    ~UART() = default;

    UART &init();

    friend Connectivity;
    friend void ::HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

private:
    UART_HandleTypeDef *m_handle = nullptr; // UART 句柄
    // 全局 UART 实例映射，便于在中断回调中找到对应的实例
    static std::map<USART_TypeDef *, UART *> uart_map;

    // 实现 Connectivity 接口的发送函数
    EsfStatus _sendMessageImpl();
    EsfStatus _receiveMessageImpl();
    EsfStatus _sendMessageDmaImpl();
    EsfStatus _receiveMessageDmaImpl();

    EsfStatus _rxCallback(uint16_t size);
};

} // namespace base
} // namespace esf

#endif /* HAL_UART_MODULE_ENABLED */