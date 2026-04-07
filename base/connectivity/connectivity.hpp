#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "FreeRTOS.h"

#include "core/status.hpp"

#include <functional>

namespace esf
{
namespace base
{
/**
 * @brief 通信接口基类，其他通信方式均继承该接口
 *
 * @tparam Derived 基类
 * @tparam MessageReceive 接收消息结构
 * @tparam MessageSend 发送消息结构
 *
 * @note 使用 CRTP 模式，替换纯虚函数。具有两个队列，作用如下：
 * Read Queue：从该队列读取消息进行发送，即 Send Queue；
 * Write Queue：接收到的消息写入该队列，即 Receive Queue。
 */
template <typename Derived,
          typename MessageReceive = typename Derived::MessageReceive,
          typename MessageSend = typename Derived::MessageSend>
class Connectivity {
public:
    enum class DMA : uint8_t { DISABLE = 0, TX = 1, RX = 2, TX_RX = 3 };
    /**
     * @brief 发送消息接口
     *
     * @param message 待发送消息
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    virtual EsfStatus sendMessage(MessageSend &message) final
    {
        if (static_cast<uint8_t>(m_dma) & static_cast<uint8_t>(DMA::TX)) {
            return static_cast<Derived *>(this)->_sendMessageDmaImpl(message);
        }
        return static_cast<Derived *>(this)->_sendMessageImpl(message);
    }
    /**
     * @brief 接收消息接口
     *
     * @param message 接收消息存储位置
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    virtual EsfStatus receiveMessage(MessageReceive &message) final
    {
        if (static_cast<uint8_t>(m_dma) & static_cast<uint8_t>(DMA::RX)) {
            return static_cast<Derived *>(this)->_receiveMessageDmaImpl(message);
        }
        return static_cast<Derived *>(this)->_receiveMessageImpl(message);
    }
    /**
     * @brief 设置接收回调函数
     *
     * @param callback 接收回调函数，参数为 BaseType_t*，用于指示是否需要切换到更高优先级的任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus setRxCallback(std::function<EsfStatus(MessageReceive &)> callback)
    {
        m_rxCallbackFunc = callback;
        return ESF_SUCCESS;
    }
    /**
     * @brief 设置发送回调函数
     *
     * @param callback 发送回调函数，参数为 BaseType_t*，用于指示是否需要切换到更高优先级的任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus setTxCallback(std::function<EsfStatus(MessageReceive &)> callback)
    {
        m_txCallbackFunc = callback;
        return ESF_SUCCESS;
    }

    MessageReceive *getReceiveData()
    {
        return &m_receiveData;
    }

    MessageSend *getSendData()
    {
        return &m_sendData;
    }

protected:
    Connectivity(DMA dma)
        : m_dma(dma) {};
    virtual ~Connectivity() = default;

    MessageReceive m_receiveData; // 接收到的数据
    MessageSend m_sendData;

    EsfStatus m_sendErrCode;    // 发送错误码
    EsfStatus m_receiveErrCode; // 接收错误码

    std::function<EsfStatus(MessageReceive &)> m_rxCallbackFunc; // 接收回调函数，用于根据接收的消息通知其他模块进行处理
    std::function<EsfStatus(MessageSend &)> m_txCallbackFunc;

    DMA m_dma;
};

/**
 * @brief 具有通信接口的基类，其他模块继承该类以获得通信能力
 *
 * @tparam ConnectivityType 具体的通信接口类型，如 UART、SPI 等
 */
template <typename ConnectivityType> class WithConnectivity {
public:
    WithConnectivity(ConnectivityType &connectivity)
        : m_connectivity(connectivity) {};
    ~WithConnectivity() = default;

protected:
    ConnectivityType &m_connectivity;
};

} // namespace base
} // namespace esf