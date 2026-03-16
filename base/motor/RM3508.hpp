#pragma once

#include "motor/motor.hpp"
#include "status.hpp"

#include <map>

namespace esf
{
namespace base
{

struct RM3508State {
    float position;
    float velocity;
    float current;
    float temperate;
};
class RM3508 : public Motor<RM3508, RM3508State, uint16_t> {
public:
    RM3508(uint16_t id, float reduration_ratio = 3591.0f / 17.0f, Direction direction = Direction::CCW);
    ~RM3508() = default;
    template <typename ConnectivityType> friend class RM3508Helper;
};

template <typename ConnectivityType>
class RM3508Helper : public MotorHelper<RM3508Helper<ConnectivityType>, RM3508, ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message;

    enum class RM3508HelperId : uint16_t { MOTOR_1_TO_4 = 0x200, MOTOR_5_TO_9 = 0x1ff };

    RM3508Helper(ConnectivityType &connectivity, RM3508HelperId id);
    ~RM3508Helper() = default;

private:
    std::map<uint16_t, RM3508 *> m_motor_map;

    ConnectivityMessageType m_receive_frame, m_send_frame;

    RM3508HelperId m_id;

    EsfStatus _registerMotorImpl(RM3508 &motor)
    {
        if ((m_id == RM3508HelperId::MOTOR_1_TO_4 && (motor.id() < 0 || motor.id() > 4)) ||
            (m_id == RM3508HelperId::MOTOR_5_TO_9 && (motor.id() < 5 || motor.id() > 9))) {
            return ESF_MOTOR_HELPER_REGISTER_ERROR;
        }
        m_motor_map[motor.id()] = &motor;
        return ESF_SUCCESS;
    }
    EsfStatus _encodeMessageImpl();
    EsfStatus _decodeMessageImpl(ConnectivityMessageType &message);
};

} // namespace base
} // namespace esf