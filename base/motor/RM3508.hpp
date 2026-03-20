#pragma once

#include "motor/motor.hpp"

namespace esf
{
namespace base
{

struct RM3508State : public BasicMotorState {
    float rotor_position; // 转子角度
    float rotor_velocity; // 转子速度
    float rotor_current;  // 转矩电流
};
class RM3508 : public Motor<RM3508, RM3508State, uint16_t> {
public:
    RM3508(uint32_t id, float reduration_ratio = 3591.0f / 17.0f, Direction direction = Direction::CCW);
    ~RM3508() = default;
    template <typename ConnectivityType> friend class RM3508Helper;

private:
    float m_current_param; // 电机电流参数，表明每 Nm 输出多大力矩
    int m_rotor_count = 0; // 转子圈数
};

template <typename ConnectivityType>
class RM3508Helper : public MotorHelper<RM3508Helper<ConnectivityType>, RM3508, ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message;

    enum class RM3508HelperId : uint16_t { MOTOR_1_TO_4 = 0x200, MOTOR_5_TO_9 = 0x1ff };

    RM3508Helper(ConnectivityType &connectivity, RM3508HelperId id);
    ~RM3508Helper() = default;

private:
    ConnectivityMessageType m_receive_frame, m_send_frame;

    RM3508HelperId m_id;

    EsfStatus _registerMotorImpl(RM3508 &motor)
    {
        if ((m_id == RM3508HelperId::MOTOR_1_TO_4 && (motor.id() < 0 || motor.id() > 4)) ||
            (m_id == RM3508HelperId::MOTOR_5_TO_9 && (motor.id() < 5 || motor.id() > 9))) {
            return ESF_MOTOR_HELPER_REGISTER_ERROR;
        }
        this->m_motor_map[motor.id()] = motor;
        return ESF_SUCCESS;
    }
    EsfStatus _encodeMessageImpl();
    EsfStatus _decodeMessageImpl(ConnectivityMessageType &message);
};

} // namespace base
} // namespace esf