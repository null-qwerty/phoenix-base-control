#pragma once

#include "RM3508.hpp"
#include "status.hpp"
#include <cmath>
#include <cstring>

namespace esf
{
namespace base
{
RM3508::RM3508(uint32_t id, float reduration_ratio, Direction direction)
    : Motor<RM3508, RM3508State, uint16_t>(id, reduration_ratio, direction)
    , m_current_param(0.3f / (3591.0f / 17.0f) * reduration_ratio)
{
    m_zero.position = 0.0f;
    m_zero.velocity = 0.0f;
    m_zero.torque = 0.0f;
    m_zero.temperature = 0.0f;

    m_min.position = -infinity();
    m_min.velocity = -469.0f * 2 * M_PI / 60 / (3591.0f / 17.0f) * reduration_ratio;
    m_min.torque = -10.0f * m_current_param;

    m_max.position = infinity();
    m_max.velocity = 469.0f * 2 * M_PI / 60 / (3591.0f / 17.0f) * reduration_ratio;
    m_max.torque = 10.0f * m_current_param;
}

template <typename ConnectivityType>
RM3508Helper<ConnectivityType>::RM3508Helper(ConnectivityType &connectivity, RM3508HelperId id)
    : MotorHelper<RM3508Helper<ConnectivityType>, RM3508, ConnectivityType>(connectivity)
    , m_id(id)
{
}
} // namespace base
} // namespace esf

#include "connectivity/fdcan.hpp"
#ifdef HAL_FDCAN_MODULE_ENABLED
namespace esf
{
namespace base
{

template <>
RM3508Helper<FDCAN>::RM3508Helper(FDCAN &connectivity, RM3508HelperId id)
    : MotorHelper<RM3508Helper<FDCAN>, RM3508, FDCAN>(connectivity)
    , m_id(id)
{
    m_send_frame.id = static_cast<uint32_t>(m_id);
    m_send_frame.is_ext = false;
    m_send_frame.size = 8;
    m_send_frame.data = new uint8_t[8];
    memset(m_send_frame.data, 0, 8);

    m_receive_frame.data = new uint8_t[8];

    m_connectivity.pushToSendQueue(m_send_frame);
    m_connectivity.pushToReceiveQueue(m_receive_frame);
}

template <> EsfStatus RM3508Helper<FDCAN>::_encodeMessageImpl()
{
    uint8_t index_offset = m_id == RM3508HelperId::MOTOR_1_TO_4 ? 1 : 5;
    for (auto &motor : m_motor_map) {
        uint8_t index = motor.first - index_offset;
        uint16_t current = static_cast<int8_t>(motor.second.m_direction) * motor.second.commend();
        m_send_frame.data[index] = (current >> 8) & 0xff;
        m_send_frame.data[index + 1] = (current >> 0) & 0xff;
    }
    return m_connectivity.pushToSendQueue(m_send_frame);
}

template <> EsfStatus RM3508Helper<FDCAN>::_decodeMessageImpl(FDCAN::Message &message)
{
    if (m_motor_map.find(message.id - 0x200) == m_motor_map.end()) {
        m_connectivity.pushToReceiveQueue(m_receive_frame);
        return ESF_MOTOR_NOT_REGISTERD_TO_HELPER;
    }
    auto &motor = m_motor_map[message.id - 0x200];
    auto data = message.data;
    auto last_rotor_position = motor.state().rotor_position; // 记录上次转子位置
    // 记录原始信息，RM3508 原始信息均为转子信息
    motor.state().rotor_position = 1.0f * static_cast<float>(motor.m_direction) *
                                   static_cast<float>((static_cast<uint16_t>(data[0]) << 8) | data[1]) * 2.0f * M_PI /
                                   8191.0f;
    motor.state().rotor_velocity = 1.0f * static_cast<float>(motor.m_direction) *
                                   static_cast<float>((static_cast<int16_t>(data[2]) << 8) | data[3]);
    motor.state().rotor_current = 1.0f * static_cast<float>(motor.m_direction) *
                                  static_cast<float>(static_cast<int16_t>(data[4] << 8) | data[5]) / 16384.0f * 10.0f;
    motor.state().temperature = static_cast<float>(data[6]);
    // 计算输出轴数据
    auto rotor_position_diff = motor.state().rotor_position - last_rotor_position;
    // 判断转子位置是否过零点
    if (rotor_position_diff > 0 && motor.state().velocity < 0) {
        motor.m_rotor_count--;
    } else if (rotor_position_diff < 0 && motor.state().velocity > 0) {
        motor.m_rotor_count++;
    }
    motor.state().position = (motor.m_rotor_count * 2.0f * M_PI + motor.state().rotor_position) /
                             motor.m_reduction_ratio;
    motor.state().velocity = motor.state().rotor_velocity / motor.m_reduction_ratio;
    motor.state().torque = motor.state().rotor_current * motor.m_current_param;

    m_connectivity.pushToReceiveQueue(m_receive_frame);
    return ESF_SUCCESS;
}

template class RM3508Helper<FDCAN>;
} // namespace base
} // namespace esf

#endif