#pragma once

#include "connectivity/connectivity.hpp"
#include "core/status.hpp"

namespace esf
{
namespace base
{
template <typename Derived, typename MotorType, typename ConnectivityType>
class MotorHelper : public WithConnectivity<ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message;
    using MotorStateType = typename MotorType::State;
    using MotorCommendType = typename MotorType::Commend;

    MotorHelper(ConnectivityType &connectivity)
        : WithConnectivity<ConnectivityType>(connectivity)
    {
    }
    ~MotorHelper() = default;
    virtual EsfStatus registerMotor(MotorType &motor) final
    {
        return static_cast<Derived *>(this)->_registerMotorImpl(motor);
    }
    virtual EsfStatus encodeMessage() final
    {
        return static_cast<Derived *>(this)->_encodeMessageImpl();
    }
    virtual EsfStatus decodeMessage(ConnectivityMessageType &message) final
    {
        return static_cast<Derived *>(this)->_decodeMessageImpl(message);
    }
};

template <typename Derived, typename StateType = Derived::State, typename CommendType = Derived::Commend> class Motor {
public:
    using State = StateType;
    using Commend = CommendType;

    enum class Direction : int8_t { CW = -1, CCW = 1 };

    Motor(uint16_t id, float reduration_ratio, Direction direction)
        : m_id(id)
        , m_reduction_ratio(reduration_ratio)
        , m_direction(direction)
    {
    }

    ~Motor() = default;

    Motor &setZeroState(State zero_state)
    {
        m_zero = zero_state;
    }

    Motor &setSoftLimit(State min, State max)
    {
        m_min = min;
        m_max = max;
    }

    State &state()
    {
        return m_state;
    }

    Commend &commend()
    {
        return m_commend;
    }

    uint16_t id()
    {
        return m_id;
    }

protected:
    uint16_t m_id;
    float m_reduction_ratio;

    State m_state, m_zero, m_max, m_min;
    Commend m_commend;

    Direction m_direction;
};
} // namespace base
} // namespace esf