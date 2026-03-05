#pragma once

#include "connectivity.hpp"
#include "errcode.hpp"
#include "portmacro.h"
#ifdef HAL_FDCAN_MODULE_ENABLED
#include <map>

namespace esf
{
namespace base
{
struct FdcanMessage {
    uint8_t *data;
    size_t size;
    uint32_t id;
};

class FDCAN : public Connectivity<FDCAN, FdcanMessage> {
public:
    using Message = FdcanMessage;

    FDCAN(FDCAN_HandleTypeDef &handle, FDCAN_FilterTypeDef &filter);
    ~FDCAN() = default;
    FDCAN &init();

    friend void ::HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
    friend void ::HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);

private:
    FDCAN_HandleTypeDef *m_handle = nullptr;
    FDCAN_FilterTypeDef *m_filter = nullptr;

    static std::map<FDCAN_GlobalTypeDef *, FDCAN *> fdcan_map;

    uint8_t m_receiveData[64] = { 0 };

    EsfReturnType _sendMessageImpl(MessageType type);
    EsfReturnType _receiveMessageImpl(MessageType type);
};
} // namespace base
} // namespace esf
#endif /* HAL_FDCAN_MODULE_ENABLED */