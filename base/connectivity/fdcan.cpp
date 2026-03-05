#include "fdcan.hpp"
#include "errcode.hpp"
#include "portmacro.h"
#ifdef HAL_FDCAN_MODULE_ENABLED

void MX_FDCAN1_Init(void) __attribute__((weak));
void MX_FDCAN2_Init(void) __attribute__((weak));
void MX_FDCAN3_Init(void) __attribute__((weak));

namespace esf
{
namespace base
{
FDCAN::FDCAN(FDCAN_HandleTypeDef &handle, FDCAN_FilterTypeDef &filter)
    : m_handle(&handle)
    , m_filter(&filter)
{
    fdcan_map[handle.Instance] = this;
}
FDCAN &FDCAN::init()
{
    if (m_handle->Instance == FDCAN1 && &MX_FDCAN1_Init != nullptr) {
        MX_FDCAN1_Init();
    } else if (m_handle->Instance == FDCAN2 && &MX_FDCAN2_Init != nullptr) {
        MX_FDCAN2_Init();
    } else if (m_handle->Instance == FDCAN3 && &MX_FDCAN3_Init != nullptr) {
        MX_FDCAN3_Init();
    }

    HAL_FDCAN_ConfigFilter(m_handle, m_filter);
    HAL_FDCAN_Start(m_handle);
    HAL_FDCAN_ActivateNotification(m_handle,
                                   m_filter->FilterConfig == FDCAN_FILTER_TO_RXFIFO0 ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE :
                                                                                       FDCAN_IT_RX_FIFO1_NEW_MESSAGE,
                                   0);
    return *this;
}

EsfReturnType FDCAN::_sendMessageImpl(MessageType type)
{
    FDCAN_TxHeaderTypeDef tx_header = { 0 };

    Message message;
    m_sendErrCode = this->read_queue.pop(message);
    if (m_sendErrCode != ESF_SUCCESS) {
        return m_sendErrCode;
    }

    tx_header.IdType = (type == MessageType::CAN_EXT) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    tx_header.Identifier = message.id;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = (message.size <= 8) ? FDCAN_DLC_BYTES_8 : FDCAN_DLC_BYTES_64;

    auto halerrcode = HAL_FDCAN_AddMessageToTxFifoQ(m_handle, &tx_header, message.data);
    if (halerrcode != HAL_OK) {
        m_sendErrCode = ESF_CONNECTIVITY_SEND_ERROR;
    }

    return m_sendErrCode;
}

EsfReturnType FDCAN::_receiveMessageImpl(MessageType type)
{
    FDCAN_RxHeaderTypeDef rx_header = { 0 };

    rx_header.IdType = (type == MessageType::CAN_EXT) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;

    auto halerrcode = HAL_FDCAN_GetRxMessage(
        m_handle, (m_filter->FilterConfig == FDCAN_FILTER_TO_RXFIFO0) ? FDCAN_RX_FIFO0 : FDCAN_RX_FIFO1, &rx_header,
        m_receiveData);
    if (halerrcode != HAL_OK) {
        m_receiveErrCode = ESF_CONNECTIVITY_RECEIVE_ERROR;
        return m_receiveErrCode;
    }

    Message message;
    message.id = rx_header.Identifier;
    message.size = rx_header.DataLength == FDCAN_DLC_BYTES_8 ? 8 : 64;
    message.data = m_receiveData;

    m_receiveErrCode = this->write_queue.push(message);

    return m_receiveErrCode;
}

std::map<FDCAN_GlobalTypeDef *, FDCAN *> FDCAN::fdcan_map;
} // namespace base
} // namespace esf

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    using namespace esf::base;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    auto errcode = FDCAN::fdcan_map[hfdcan->Instance]->m_rxCallbackFunc(&xHigherPriorityTaskWoken);
}

#endif /* HAL_FDCAN_MODULE_ENABLED */