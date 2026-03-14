#include "spi.hpp"
#include "status.hpp"

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

EsfStatus SPI::_sendMessageImpl()
{
    // 从发送队列中获取消息进行发送
    Message message;
    this->m_sendErrCode = this->read_queue.pop(message);
    if (this->m_sendErrCode != ESF_SUCCESS) {
        return this->m_sendErrCode;
    }
    // 发送消息
    if (HAL_SPI_Transmit_DMA(m_handle, message.data, message.size) != HAL_OK) {
        return (m_sendErrCode = ESF_CONNECTIVITY_SEND_ERROR);
    }
    return m_sendErrCode;
}

EsfStatus SPI::_receiveMessageImpl()
{
    // 从接收队列中获取接收消息的信息
    Message message;
    this->m_receiveErrCode = this->write_queue.pop(message);
    if (this->m_receiveErrCode != ESF_SUCCESS) {
        return this->m_receiveErrCode;
    }
    auto errcode = HAL_SPI_Receive_DMA(m_handle, message.data, message.size);
    if (errcode == HAL_OK) {
        this->m_receiveErrCode = ESF_SUCCESS;
    } else {
        this->m_receiveErrCode = ESF_CONNECTIVITY_RECEIVE_ERROR;
    }
    // 存入缓存区，用于中断
    this->m_receiveData = message;

    return this->m_receiveErrCode;
}

EsfStatus SPI::_rxCallback(uint16_t size)
{
    // 调用自定义接收回调函数
    if (size >= this->m_receiveData.size && this->m_rxCallbackFunc) {
        this->m_receiveErrCode = this->m_rxCallbackFunc(this->m_receiveData);
    }
    return m_receiveErrCode;
}

EsfStatus SPI::_txCallback(uint16_t size)
{
    // 调用自定义接收回调函数
    if (size >= this->m_sendData.size && this->m_txCallbackFunc) {
        this->m_sendErrCode = this->m_txCallbackFunc(this->m_sendData);
    }
    return m_sendErrCode;
}

} // namespace base
} // namespace esf

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // HAL_SPI_DMAStop(hspi);
    esf::base::SPI::spi_map[hspi->Instance]->_rxCallback(hspi->RxXferSize);
    // HAL_SPI_DMAResume(hspi);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // HAL_SPI_DMAStop(hspi);
    esf::base::SPI::spi_map[hspi->Instance]->_txCallback(hspi->TxXferSize);
    // HAL_SPI_DMAResume(hspi);
}

#endif /* HAL_SPI_MODULE_ENABLED */