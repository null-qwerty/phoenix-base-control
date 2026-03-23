#pragma once

#include "connectivity.hpp"
#ifdef HAL_SPI_MODULE_ENABLED
#include "portmacro.h"
#include "spi.h"

#include <map>

namespace esf
{
namespace base
{
struct SpiMessage {
    uint8_t *data;
    size_t size;
};

class SPI : public Connectivity<SPI, SpiMessage> {
public:
    using Message = SpiMessage; // 消息类型定义
    SPI(SPI_HandleTypeDef &handle, DMA dma);
    ~SPI() = default;
    SPI &init();

    friend Connectivity;
    friend void ::HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
    friend void ::HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

private:
    SPI_HandleTypeDef *m_handle = nullptr; // SPI 句柄
    // 全局 SPI 实例映射，便于在中断回调中找到对应的实例
    static std::map<SPI_TypeDef *, SPI *> spi_map;
    // 实现 Connectivity 接口的发送函数
    EsfStatus _sendMessageImpl();
    EsfStatus _receiveMessageImpl();
    EsfStatus _sendMessageDmaImpl();
    EsfStatus _receiveMessageDmaImpl();

    EsfStatus _rxCallback(uint16_t size);
    EsfStatus _txCallback(uint16_t size);
};

} // namespace base
} // namespace esf

#endif /* HAL_SPI_MODULE_ENABLED */