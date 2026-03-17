#pragma once

#include "connectivity/connectivity.hpp"
#include "core/status.hpp"

namespace esf
{
namespace base
{

/**
 * @brief 基本电机信息
 *
 */
struct BasicMotorState {
    float position;    // 输出位置，单位 rad
    float velocity;    // 输出速度，单位 rad/s
    float torque;      // 输出力矩，单位 Nm
    float temperature; // 温度，单位 °C
};

/**
 * @brief 电机通信辅助类，用于通信，使用 CRTP 模式进行静态多态
 *
 * @tparam Derived 派生类类型
 * @tparam MotorType 电机类型
 * @tparam ConnectivityType 通信接口类型
 */
template <typename Derived, typename MotorType, typename ConnectivityType>
class MotorHelper : public WithConnectivity<ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message; // 通信接口消息类型
    using MotorStateType = typename MotorType::State;                   // 电机状态
    using MotorCommendType = typename MotorType::Commend;               // 电机控制指令

    /**
     * @brief 构造函数
     *
     * @param connectivity 通信接口
     */
    MotorHelper(ConnectivityType &connectivity)
        : WithConnectivity<ConnectivityType>(connectivity)
    {
    }
    /**
     * @brief 析构函数，默认析构
     *
     */
    ~MotorHelper() = default;
    /**
     * @brief 注册电机到内部
     *
     * @param motor 需要注册的电机
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他返回参考 core/status.hpp
     * @note CRTP 接口，派生类需要实现 `EsfStatus _registerMotorImpl(MotorType)`
     */
    virtual EsfStatus registerMotor(MotorType &motor) final
    {
        return static_cast<Derived *>(this)->_registerMotorImpl(motor);
    }
    /**
     * @brief 反序列化电机控制信息为通信消息，并进行发送
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他返回参考 core/status.hpp
     * @note CRTP 接口，派生类需要实现 `EsfStatus _encodeMessageImpl()`
     */
    virtual EsfStatus encodeMessage() final
    {
        return static_cast<Derived *>(this)->_encodeMessageImpl();
    }
    /**
     * @brief 序列化接收的消息为电机状态量
     *
     * @param message 接收到的消息
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他返回参考 core/status.hpp
     * @note CRTP 接口，派生类需要实现 `EsfStatus _decodeMessageImpl(ConnectivityMessageType)`
     */
    virtual EsfStatus decodeMessage(ConnectivityMessageType &message) final
    {
        return static_cast<Derived *>(this)->_decodeMessageImpl(message);
    }
};

/**
 * @brief 电机类，用于存储电机属性、状态信息和控制信息，使用 CRTP 模式进行静态多态
 *
 * @tparam Derived 派生类
 * @tparam StateType 电机状态量类型
 * @tparam CommendType 电机控制量类型
 */
template <typename Derived, typename StateType = Derived::State, typename CommendType = Derived::Commend> class Motor {
public:
    using State = StateType;
    using Commend = CommendType;

    enum class Direction : int8_t { CW = -1, CCW = 1 }; // 电机旋转正方向，默认逆时针（CCW）为正

    /**
     * @brief 构造函数
     *
     * @param id 电机 ID
     * @param reduration_ratio 电机减速比
     * @param direction 电机旋转正方向
     */
    Motor(uint16_t id, float reduration_ratio, Direction direction)
        : m_id(id)
        , m_reduction_ratio(reduration_ratio)
        , m_direction(direction)
    {
    }

    /**
     * @brief 析构函数，默认析构
     *
     */
    ~Motor() = default;

    /**
     * @brief 设置电机软零点状态
     *
     * @param zero_state 零点状态
     * @return Motor& 自身引用
     */
    Motor &setZeroState(State zero_state)
    {
        m_zero = zero_state;
    }

    /**
     * @brief 设置电机软限制
     *
     * @param min 最小值
     * @param max 最大值
     * @return Motor& 自身引用
     */
    Motor &setSoftLimit(State min, State max)
    {
        m_min = min;
        m_max = max;
    }

    /**
     * @brief 获取电机状态
     *
     * @return State& 电机状态
     */
    State &state()
    {
        return m_state;
    }

    /**
     * @brief 获取电机控制信息
     *
     * @return Commend& 控制信息
     */
    Commend &commend()
    {
        return m_commend;
    }

    /**
     * @brief 获取电机 ID
     *
     * @return uint16_t 电机 ID
     */
    uint16_t id()
    {
        return m_id;
    }

protected:
    uint16_t m_id;           // 电机 ID
    float m_reduction_ratio; // 减速比

    State m_state, m_zero, m_max, m_min; // 状态、零点和限制
    Commend m_commend;                   // 控制信息

    Direction m_direction; // 正方向
};
} // namespace base
} // namespace esf