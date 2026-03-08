#include "spi.hpp"
#include "errcode.hpp"

#ifdef HAL_SPI_MODULE_ENABLED

#ifdef SPI1
void MX_SPI1_Init(void) __attribute__((weak));
#endif
#ifdef SPI2
void MX_SPI2_Init(void) __attribute__((weak));
#endif
#ifdef SPI3
void MX_SPI3_Init(void) __attribute__((weak));
#endif
#ifdef SPI4
void MX_SPI4_Init(void) __attribute__((weak));
#endif
#ifdef SPI5
void MX_SPI5_Init(void) __attribute__((weak));
#endif
#ifdef SPI6
void MX_SPI6_Init(void) __attribute__((weak));
#endif

#define SpiInit(handle)                                                   \
    if (m_handle->Instance == handle && &MX_##handle##_Init != nullptr) { \
        MX_##handle##_Init();                                             \
    }

namespace esf
{
namespace base
{
std::map<SPI_TypeDef *, SPI *> SPI::spi_map;

SPI::SPI(SPI_HandleTypeDef &handle)
    : m_handle(&handle)
{
    spi_map[handle.Instance] = this; // 将 SPI 实例添加到映射中
    // 初始化接收缓冲区
    m_receiveData.read().data = new uint8_t[128];
    m_receiveData.swap();
    m_receiveData.read().data = new uint8_t[128];
}

SPI &SPI::init()
{
    // 根据 SPI 句柄的实例选择对应的初始化函数进行初始化
#ifdef SPI1
    SpiInit(SPI1);
#endif
#ifdef SPI2
    SpiInit(SPI2);
#endif
#ifdef SPI3
    SpiInit(SPI3);
#endif
#ifdef SPI4
    SpiInit(SPI4);
#endif
#ifdef SPI5
    SpiInit(SPI5);
#endif
#ifdef SPI6
    SpiInit(SPI6);
#endif

    return *this;
}

EsfStatus SPI::_sendMessageImpl(MessageType type)
{
    // 从发送队列中获取消息进行发送
    Message message;
    m_sendErrCode = this->read_queue.pop(message);
    if (m_sendErrCode != ESF_SUCCESS) {
        return m_sendErrCode;
    }
    // 发送消息
    if (HAL_SPI_Transmit_DMA(m_handle, message.data, message.size) != HAL_OK) {
        return (m_sendErrCode = ESF_CONNECTIVITY_SEND_ERROR);
    }
    return m_sendErrCode;
}

EsfStatus SPI::_receiveMessageImpl(MessageType type)
{
    // 启动 DMA 接收
    if (HAL_SPI_Receive_DMA(m_handle, m_receiveData.write().data, m_receiveData.write().size) != HAL_OK) {
        return (m_receiveErrCode = ESF_CONNECTIVITY_RECEIVE_ERROR);
    }
    return ESF_SUCCESS;
}

EsfStatus SPI::_rxCallback(uint16_t size)
{
    // 切换接收缓冲区
    m_receiveData.swap();
    // 启动新的 DMA 接收
    HAL_SPI_Receive_DMA(m_handle, m_receiveData.write().data, m_receiveData.write().size);
    // 将接收到的数据写入接收队列
    Message message = m_receiveData.read();
    m_receiveErrCode = this->write_queue.pushFromISR(message);
    if (m_receiveErrCode != ESF_SUCCESS) {
        return m_receiveErrCode;
    }
    // 调用自定义接收回调函数
    return this->m_rxCallbackFunc(message);
}
} // namespace base
} // namespace esf

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    auto spi = esf::base::SPI::spi_map[hspi->Instance];
    spi->_rxCallback(hspi->RxXferSize);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    ;
}

#endif /* HAL_SPI_MODULE_ENABLED */