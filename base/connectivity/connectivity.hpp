#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "FreeRTOS/queue.hpp"
#include "core/doublebuffer.hpp"
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
 * @tparam Message 队列元素类型，即消息类型
 *
 * @note 使用 CRTP 模式，替换纯虚函数。具有两个队列，作用如下：
 * Read Queue：从该队列读取消息进行发送，即 Send Queue；
 * Write Queue：接收到的消息写入该队列，即 Receive Queue。
 */
template <typename Derived, typename Message = typename Derived::Message>
class Connectivity : public ReadQueueBase<Message>, public WriteQueueBase<Message> {
public:
    enum class DMA : uint8_t { DISABLE = 0, TX = 1, RX = 2, TX_RX = 3 };
    /**
     * @brief 发送消息接口
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    virtual EsfStatus sendMessage() final
    {
        if (static_cast<uint8_t>(m_dma) & static_cast<uint8_t>(DMA::TX)) {
            return static_cast<Derived *>(this)->_sendMessageDmaImpl();
        }
        return static_cast<Derived *>(this)->_sendMessageImpl();
    }
    /**
     * @brief 接收消息接口
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    virtual EsfStatus receiveMessage() final
    {
        if (static_cast<uint8_t>(m_dma) & static_cast<uint8_t>(DMA::RX)) {
            return static_cast<Derived *>(this)->_receiveMessageDmaImpl();
        }
        return static_cast<Derived *>(this)->_receiveMessageImpl();
    }

    /**
     * @brief 向发送队列添加消息
     *
     * @param message 需要添加的消息
     * @param fromISR 是否从中断调用
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus pushToSendQueue(Message message, bool fromISR = false)
    {
        if (fromISR) {
            return this->read_queue.pushFromISR(message);
        }
        return this->read_queue.push(message);
    }
    /**
     * @brief 从接收队列弹出消息
     *
     * @param message 消息存放的位置
     * @param fromISR 是否从中断调用
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus pushToReceiveQueue(Message message, bool fromISR = false)
    {
        if (fromISR) {
            return this->write_queue.pushFromISR(message);
        }
        return this->write_queue.push(message);
    }
    /**
     * @brief 设置接收回调函数
     *
     * @param callback 接收回调函数，参数为 BaseType_t*，用于指示是否需要切换到更高优先级的任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus setRxCallback(std::function<EsfStatus(Message &)> callback)
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
    EsfStatus setTxCallback(std::function<EsfStatus(Message &)> callback)
    {
        m_txCallbackFunc = callback;
        return ESF_SUCCESS;
    }

protected:
    Connectivity(DMA dma)
        : m_dma(dma) {};
    virtual ~Connectivity() = default;

    Message m_receiveData; // 接收到的数据
    Message m_sendData;

    EsfStatus m_sendErrCode;    // 发送错误码
    EsfStatus m_receiveErrCode; // 接收错误码

    std::function<EsfStatus(Message &)> m_rxCallbackFunc; // 接收回调函数，用于根据接收的消息通知其他模块进行处理
    std::function<EsfStatus(Message &)> m_txCallbackFunc;

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