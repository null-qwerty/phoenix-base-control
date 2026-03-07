#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "FreeRTOS/queue.hpp"
#include "core/errcode.hpp"
#include "core/doublebuffer.hpp"

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
    enum class MessageType : uint8_t { DATA_STREAM = 0, CAN_STD, CAN_EXT, UART, SPI, I2C, RS485, COUNT };

    /**
     * @brief 发送消息接口
     *
     * @param type 通信类型
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    virtual EsfStatus sendMessage(MessageType type) final;
    /**
     * @brief 接收消息接口
     *
     * @param type 通信类型
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    virtual EsfStatus receiveMessage(MessageType type) final;

    /**
     * @brief 向发送队列添加消息
     *
     * @param message 需要添加的消息
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfStatus pushToSendQueue(Message message);
    /**
     * @brief 从接收队列弹出消息
     *
     * @param message 消息存放的位置
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfStatus popFromReceiveQueue(Message &message);
    /**
     * @brief 设置接收数据大小
     *
     * @param size 接收数据大小，单位为字节
     * @return Connectivity<Derived> 返回当前实例的引用，便于链式调用
     */
    Connectivity<Derived, Message> setReceiveDataSize(size_t size);
    /**
     * @brief 设置接收回调函数
     *
     * @param callback 接收回调函数，参数为 BaseType_t*，用于指示是否需要切换到更高优先级的任务
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/errcode.hpp
     */
    EsfStatus setRxCallback(std::function<EsfStatus(BaseType_t *)> &callback);

protected:
    Connectivity() = default;
    virtual ~Connectivity() = default;

    DoubleBuffer<Message> m_receiveData; // 接收数据缓冲区

    EsfStatus m_sendErrCode; // 发送错误码
    EsfStatus m_receiveErrCode; // 接收错误码

    std::function<EsfStatus(Message &)> m_rxCallbackFunc; // 接收回调函数，用于根据接收的消息通知其他模块进行处理
};

} // namespace base
} // namespace esf