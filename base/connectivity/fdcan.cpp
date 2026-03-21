#include "fdcan.hpp"
#include "portmacro.h"
#ifdef HAL_FDCAN_MODULE_ENABLED

// 初始化函数声明，弱定义，用于连接 FreeRTOS 的初始化函数
#ifdef FDCAN1
void MX_FDCAN1_Init(void) __attribute__((weak));
#endif
#ifdef FDCAN2
void MX_FDCAN2_Init(void) __attribute__((weak));
#endif
#ifdef FDCAN3
void MX_FDCAN3_Init(void) __attribute__((weak));
#endif

namespace esf
{
namespace base
{
FDCAN &FDCAN::init()
{
// 根据 FDCAN 句柄的实例选择对应的初始化函数进行初始化
#ifdef FDCAN1
    if (m_handle->Instance == FDCAN1 && &MX_FDCAN1_Init != nullptr) {
        MX_FDCAN1_Init();
    }
#endif
#ifdef FDCAN2
    if (m_handle->Instance == FDCAN2 && &MX_FDCAN2_Init != nullptr) {
        MX_FDCAN2_Init();
    }
#endif
#ifdef FDCAN3
    if (m_handle->Instance == FDCAN3 && &MX_FDCAN3_Init != nullptr) {
        MX_FDCAN3_Init();
    }
#endif
    // 配置过滤器并启动 FDCAN
    HAL_FDCAN_ConfigFilter(m_handle, m_filter);
    HAL_FDCAN_Start(m_handle);
    HAL_FDCAN_ActivateNotification(m_handle,
                                   m_filter->FilterConfig == FDCAN_FILTER_TO_RXFIFO0 ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE
                                                                                     : FDCAN_IT_RX_FIFO1_NEW_MESSAGE,
                                   0);
    return *this;
}

EsfStatus FDCAN::_sendMessageImpl()
{
    FDCAN_TxHeaderTypeDef tx_header = { 0 };
    // 从发送队列中获取消息进行发送
    Message message;
    m_sendErrCode = this->read_queue.pop(message);
    if (m_sendErrCode != ESF_SUCCESS) {
        return m_sendErrCode;
    }

    tx_header.IdType = (message.is_ext == true) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    tx_header.Identifier = message.id;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = (message.size <= 8) ? FDCAN_DLC_BYTES_8 : FDCAN_DLC_BYTES_64;

    auto halerrcode = HAL_FDCAN_AddMessageToTxFifoQ(m_handle, &tx_header, message.data);
    if (halerrcode != HAL_OK) {
        m_sendErrCode = ESF_CONNECTIVITY_SEND_ERROR;
    }

    return m_sendErrCode;
}

EsfStatus FDCAN::_receiveMessageImpl()
{
    FDCAN_RxHeaderTypeDef rx_header = { 0 };

    // 从接收队列中获取信息进行接受
    Message message;
    m_receiveErrCode = this->write_queue.pop(message);
    if (m_receiveErrCode != ESF_SUCCESS) {
        return m_receiveErrCode;
    }
    auto halerrcode = HAL_FDCAN_GetRxMessage(m_handle,
                                             (m_filter->FilterConfig == FDCAN_FILTER_TO_RXFIFO0) ? FDCAN_RX_FIFO0
                                                                                                 : FDCAN_RX_FIFO1,
                                             &rx_header,
                                             message.data);
    if (halerrcode != HAL_OK) {
        m_receiveErrCode = ESF_CONNECTIVITY_RECEIVE_ERROR;
        return m_receiveErrCode;
    }
    message.id = rx_header.Identifier;
    message.size = (rx_header.DataLength > 8) ? 64 : rx_header.DataLength; // FDCAN DLC
                                                                           // 转换为实际字节
    message.is_ext = rx_header.IdType == FDCAN_EXTENDED_ID ? true : false;
    this->m_receiveData = message;

    return m_receiveErrCode;
}

EsfStatus FDCAN::_rxCallback()
{
    FDCAN_RxHeaderTypeDef rx_header = { 0 };
    auto halerrcode = HAL_FDCAN_GetRxMessage(m_handle,
                                             (m_filter->FilterConfig == FDCAN_FILTER_TO_RXFIFO0) ? FDCAN_RX_FIFO0
                                                                                                 : FDCAN_RX_FIFO1,
                                             &rx_header,
                                             m_receiveData.data);
    if (halerrcode != HAL_OK) {
        m_receiveErrCode = ESF_CONNECTIVITY_RECEIVE_ERROR;
        return m_receiveErrCode;
    }
    m_receiveData.id = rx_header.Identifier;
    m_receiveData.size = (rx_header.DataLength > 8) ? 64 : rx_header.DataLength; // FDCAN DLC
                                                                                 // 转换为实际字节
    // 调用自定义接收回调函数
    auto errcode = this->m_rxCallbackFunc(m_receiveData);
    return errcode;
}

std::map<FDCAN_GlobalTypeDef *, FDCAN *> FDCAN::fdcan_map; // 定义全局 FDCAN 实例映射
} // namespace base
} // namespace esf

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    using namespace esf::base;
    // 根据中断回调找到对应的 FDCAN 实例，并调用其接收回调函数
    auto errcode = FDCAN::fdcan_map[hfdcan->Instance]->_rxCallback();
}

#endif /* HAL_FDCAN_MODULE_ENABLED */