#pragma once

#include "RM3508.hpp"
#include "status.hpp"
#include <cmath>
#include <cstring>

namespace esf
{
namespace base
{
RM3508::RM3508(uint16_t id, float reduration_ratio, Direction direction)
    : Motor<RM3508, RM3508State, uint16_t>(id, reduration_ratio, direction)
{
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
        uint16_t current = motor.second->commend();
        m_send_frame.data[index] = (current >> 8) & 0xff;
        m_send_frame.data[index + 1] = (current >> 0) & 0xff;
    }
    return m_connectivity.pushToSendQueue(m_send_frame);
}

template <> EsfStatus RM3508Helper<FDCAN>::_decodeMessageImpl(FDCAN::Message &message)
{
    if (m_motor_map.find(message.id - 0x200) != m_motor_map.end()) {
        auto &motor = m_motor_map[message.id - 0x200];
        auto data = message.data;
        motor->state().position = 1.0f * static_cast<float>(motor->m_direction) *
                                  static_cast<float>((static_cast<uint16_t>(data[0]) << 8) | data[1]) * 2.0f * M_PI /
                                  8191.0f;
        motor->state().velocity = 1.0f * static_cast<float>(motor->m_direction) *
                                  static_cast<float>((static_cast<int16_t>(data[2]) << 8) | data[3]);
        motor->state().current = 1.0f * static_cast<float>(motor->m_direction) *
                                 static_cast<float>(static_cast<int16_t>(data[4] << 8) | data[5]) / 16384.0f * 3.0f;
        motor->state().temperate = static_cast<float>(data[6]);

        m_connectivity.pushToReceiveQueue(m_receive_frame);
        return ESF_SUCCESS;
    }
    m_connectivity.pushToReceiveQueue(m_receive_frame);
    return ESF_MOTOR_NOT_REGISTERD_TO_HELPER;
}

template class RM3508Helper<FDCAN>;
} // namespace base
} // namespace esf

#endif