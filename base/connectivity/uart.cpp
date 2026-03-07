#include "uart.hpp"
#include "errcode.hpp"
#include "stm32h7xx_hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

// 初始化函数声明，弱定义，用于连接 FreeRTOS 的初始化函数
#ifdef UART1
void MX_UART1_Init(void) __attribute__((weak));
#endif
#ifdef UART2
void MX_UART2_Init(void) __attribute__((weak));
#endif
#ifdef UART3
void MX_UART3_Init(void) __attribute__((weak));
#endif
#ifdef UART4
void MX_UART4_Init(void) __attribute__((weak));
#endif
#ifdef UART5
void MX_UART5_Init(void) __attribute__((weak));
#endif
#ifdef UART6
void MX_UART6_Init(void) __attribute__((weak));
#endif
#ifdef UART7
void MX_UART7_Init(void) __attribute__((weak));
#endif
#ifdef UART8
void MX_UART8_Init(void) __attribute__((weak));
#endif
#ifdef UART9
void MX_UART9_Init(void) __attribute__((weak));
#endif
#ifdef UART10
void MX_UART10_Init(void) __attribute__((weak));
#endif

#ifdef USART1
void MX_USART1_Init(void) __attribute__((weak));
#endif
#ifdef USART2
void MX_USART2_Init(void) __attribute__((weak));
#endif
#ifdef USART3
void MX_USART3_Init(void) __attribute__((weak));
#endif
#ifdef USART4
void MX_USART4_Init(void) __attribute__((weak));
#endif
#ifdef USART5
void MX_USART5_Init(void) __attribute__((weak));
#endif
#ifdef USART6
void MX_USART6_Init(void) __attribute__((weak));
#endif
#ifdef USART7
void MX_USART7_Init(void) __attribute__((weak));
#endif
#ifdef USART8
void MX_USART8_Init(void) __attribute__((weak));
#endif
#ifdef USART9
void MX_USART9_Init(void) __attribute__((weak));
#endif
#ifdef USART10
void MX_USART10_Init(void) __attribute__((weak));
#endif

#define UartInit(handle)                                                  \
    if (m_handle->Instance == handle && &MX_##handle##_Init != nullptr) { \
        MX_##handle##_Init();                                             \
    }

namespace esf
{
namespace base
{
std::map<USART_TypeDef *, UART *> UART::uart_map; // 定义全局 UART 实例映射

UART::UART(UART_HandleTypeDef &handle)
    : m_handle(&handle)
{
    uart_map[handle.Instance] = this; // 将 UART 实例添加到映射中
    // 初始化接收数据缓冲区
    m_receiveData.read().data = new uint8_t[128];
    m_receiveData.swap();
    m_receiveData.read().data = new uint8_t[128];
}

UART &UART::init()
{
    // 根据 UART 句柄的实例选择对应的初始化函数进行初始化
#ifdef UART1
    UartInit(UART1);
#endif
#ifdef UART2
    UartInit(UART2);
#endif
#ifdef UART3
    UartInit(UART3);
#endif
#ifdef UART4
    UartInit(UART4);
#endif
#ifdef UART5
    UartInit(UART5);
#endif
#ifdef UART6
    UartInit(UART6);
#endif
#ifdef UART7
    UartInit(UART7);
#endif
#ifdef UART8
    UartInit(UART8);
#endif
#ifdef UART9
    UartInit(UART9);
#endif
#ifdef UART10
    UartInit(UART10);
#endif
#ifdef USART1
    UartInit(USART1);
#endif
#ifdef USART2
    UartInit(USART2);
#endif
#ifdef USART3
    UartInit(USART3);
#endif
#ifdef USART4
    UartInit(USART4);
#endif
#ifdef USART5
    UartInit(USART5);
#endif
#ifdef USART6
    UartInit(USART6);
#endif
#ifdef USART7
    UartInit(USART7);
#endif
#ifdef USART8
    UartInit(USART8);
#endif
#ifdef USART9
    UartInit(USART9);
#endif
#ifdef USART10
    UartInit(USART10);
#endif
    // 启动 UART 空闲中断
    __HAL_UART_ENABLE_IT(m_handle, UART_IT_IDLE);
    // 启动 UART 接收 DMA
    HAL_UART_Receive_DMA(m_handle, m_receiveData.write().data, 128);
    return *this;
}

EsfStatus UART::_sendMessageImpl(MessageType type)
{
    // 从发送队列中获取消息进行发送
    Message message;
    m_sendErrCode = this->read_queue.pop(message);
    if (m_sendErrCode != ESF_SUCCESS) {
        return m_sendErrCode;
    }
    // 发送消息
    if (HAL_UART_Transmit_DMA(m_handle, message.data, message.size) != HAL_OK) {
        return (m_sendErrCode = ESF_CONNECTIVITY_SEND_ERROR);
    }
    return m_sendErrCode;
}

EsfStatus UART::_receiveMessageImpl(MessageType type)
{
    return ESF_SUCCESS;
}

EsfStatus UART::_rxCallback(uint16_t size)
{
    // 切换接收缓冲区
    m_receiveData.swap();
    // 启动新的 DMA 接收
    HAL_UART_Receive_DMA(m_handle, m_receiveData.write().data, 128);
    // 将接收到的数据写入接收队列
    Message message = m_receiveData.read();
    message.size = size;
    m_receiveErrCode = this->write_queue.pushFromISR(message);
    // 调用自定义接收回调函数
    auto errcode = this->m_rxCallbackFunc(message);

    return errcode;
}
} // namespace base
} // namespace esf

// uart 空闲中断
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 关闭 DMA
    HAL_UART_DMAStop(huart);
    // 调用对应 UART 实例的接收回调函数
    auto errcode = esf::base::UART::uart_map[huart->Instance]->_rxCallback(Size);
}

#endif /* HAL_UART_MODULE_ENABLED */