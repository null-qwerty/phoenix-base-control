#pragma once

#include "connectivity.hpp"
#include "portmacro.h"
#ifdef HAL_FDCAN_MODULE_ENABLED
#include <map>

#include "fdcan.h"

namespace esf
{
namespace base
{
/**
 * @brief fdcan 消息结构体
 *
 */
struct FdcanMessage {
    uint8_t *data;
    size_t size;
    uint32_t id;
    bool is_ext;
};
/**
 * @brief fdcan 通信类，继承自 Connectivity，使用 CRTP 模式实现
 *
 */
class FDCAN : public Connectivity<FDCAN, FdcanMessage> {
public:
    using Message = FdcanMessage; // 消息类型定义

    /**
     * @brief 构造函数
     *
     * @param handle FDCAN 句柄
     * @param filter FDCAN 过滤器配置
     */
    FDCAN(FDCAN_HandleTypeDef &handle, FDCAN_FilterTypeDef &filter);
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
    EsfStatus _sendMessageImpl();
    EsfStatus _receiveMessageImpl();

    EsfStatus _rxCallback();
};
} // namespace base
} // namespace esf
#endif /* HAL_FDCAN_MODULE_ENABLED */