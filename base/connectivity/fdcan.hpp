#pragma once

#include "connectivity.hpp"
#ifdef HAL_FDCAN_MODULE_ENABLED
#include <map>

#include "fdcan.h"

namespace esf
{
namespace base
{
/**
 * @brief fdcan 发送消息结构体
 *
 */
struct FdcanSendMessage {
    FDCAN_TxHeaderTypeDef header = { 0 };
    uint8_t *data;
};
/**
 * @brief fdcan 接收消息结构体
 *
 */
struct FdcanReceiveMessage {
    FDCAN_RxHeaderTypeDef header = { 0 };
    uint8_t *data;
};
/**
 * @brief fdcan 通信类，继承自 Connectivity，使用 CRTP 模式实现
 *
 */
class FDCAN : public Connectivity<FDCAN, FdcanReceiveMessage, FdcanSendMessage> {
public:
    using MessageReceive = FdcanReceiveMessage; // 消息类型定义
    using MessageSend = FdcanSendMessage;

    /**
     * @brief 构造函数
     *
     * @param handle FDCAN 句柄
     * @param filter FDCAN 过滤器配置
     */
    FDCAN(FDCAN_HandleTypeDef &handle, FDCAN_FilterTypeDef &filter)
        : Connectivity<FDCAN, FdcanReceiveMessage, FdcanSendMessage>(DMA::DISABLE)
        , m_handle(&handle)
        , m_filter(&filter)
    {
        fdcan_map[handle.Instance] = this;
    }
    /**
     * @brief 析构函数
     *
     */
    ~FDCAN() = default;
    /**
     * @brief 初始化函数
     *
     * @return FDCAN& 自身引用
     */
    FDCAN &init();
    friend Connectivity;
    // 将接收回调设置为友元函数，以便在中断回调中访问类的接收回调函数
    friend void ::HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
    friend void ::HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);

private:
    FDCAN_HandleTypeDef *m_handle = nullptr; // FDCAN 句柄
    FDCAN_FilterTypeDef *m_filter = nullptr; // FDCAN 过滤器配置

    // 全局 FDCAN 实例映射，便于在中断回调中找到对应的实例
    static std::map<FDCAN_GlobalTypeDef *, FDCAN *> fdcan_map;

    // 实现 Connectivity 接口的发送和接收函数
    EsfStatus _sendMessageImpl(MessageSend &message);
    EsfStatus _receiveMessageImpl(MessageReceive &message);
    EsfStatus _sendMessageDmaImpl(MessageSend &message);
    EsfStatus _receiveMessageDmaImpl(MessageReceive &message);

    EsfStatus _rxCallback();
};
} // namespace base
} // namespace esf
#endif /* HAL_FDCAN_MODULE_ENABLED */