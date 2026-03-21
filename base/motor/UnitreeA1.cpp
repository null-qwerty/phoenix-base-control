#pragma once

#include "motor/UnitreeA1.hpp"
#include "motor/UnitreeA1protocol.hpp"
#include "status.hpp"
#include "thread.hpp"
#include <cmath>
#include <cstring>

namespace esf
{
namespace base
{
UnitreeA1::UnitreeA1(uint32_t id, float reduration_ratio, Direction direction)
    : Motor<UnitreeA1, UnitreeA1State, UnitreeA1Commend>(id, reduration_ratio, direction)
{
    m_zero.position = 0.0f;
    m_zero.velocity = 0.0f;
    m_zero.torque = 0.0f;
    m_zero.temperature = 0.0f;

    m_min.position = -infinity();
    m_min.velocity = -21.0f;
    m_min.torque = -33.5f;

    m_max.position = infinity();
    m_max.velocity = 21.0f;
    m_max.torque = 33.5f;

    MasterComdV3 *pCommedDataBuf = &(m_commend.data);
    memset(pCommedDataBuf, 0, sizeof(MasterComdV3));
    m_commend.header.id = static_cast<uint8_t>(id);
}

} // namespace base
} // namespace esf

#include "connectivity/uart.hpp"
#ifdef HAL_UART_MODULE_ENABLED
namespace esf
{
namespace base
{

template <> inline EsfStatus UnitreeA1Helper<UART>::_sendOneMotorCommend(UnitreeA1 *motor)
{
    UnitreeA1CommendData *comdPtr = &motor->m_commend;
    motor->m_commend.crc = UnitreeA1Helper::crc32(reinterpret_cast<uint32_t *>(comdPtr),
                                                  sizeof(UnitreeA1CommendData) / 4 - 1);
    // 发送 buffer 段
    m_send_frame.data = reinterpret_cast<uint8_t *>(comdPtr);
    m_send_frame.size = sizeof(UnitreeA1CommendData);
    auto errcode = m_connectivity.pushToSendQueue(m_receive_frame);
    if (errcode == ESF_SUCCESS) {
        esf::Thread::this_thread().wait();
    }
    return errcode;
}

template <> EsfStatus UnitreeA1Helper<UART>::_encodeMessageImpl()
{
    m_helper_status = ESF_SUCCESS;
    for (auto &id_motor_pair : m_motor_map) {
        auto &motor = id_motor_pair.second;
        // 根据协议转换
        motor->m_commend.data.T = 1.0 * static_cast<double>(motor->m_direction) * motor->m_commend.torque * 256;
        motor->m_commend.data.W = 1.0 * static_cast<double>(motor->m_direction) * motor->m_commend.velocity * 128;
        motor->m_commend.data.Pos = 1.0 * static_cast<double>(motor->m_direction) * motor->m_commend.position * 16384 /
                                    2 / M_PI;
        motor->m_commend.data.K_P = motor->m_commend.kp * 2048;
        motor->m_commend.data.K_W = motor->m_commend.kw * 1024;
        // 发送一个电机的控制信号，并接受反馈
        auto errcode = this->_sendOneMotorCommend(motor);
        if (!errcode) {
            m_helper_status = errcode;
        }
    }
    return m_helper_status;
}

template <> EsfStatus UnitreeA1Helper<UART>::_decodeMessageImpl(UART::Message &message)
{
    UnitreeA1StateData *statePtr = reinterpret_cast<UnitreeA1StateData *>(message.data);
    if (statePtr->crc !=
        UnitreeA1Helper::crc32(reinterpret_cast<uint32_t *>(statePtr), sizeof(UnitreeA1StateData) / 4 - 1)) {
        return ESF_MOTOR_HELPER_DATA_INVALIED;
    }
    if (this->m_motor_map.find(statePtr->header.id) == this->m_motor_map.end()) {
        return ESF_MOTOR_NOT_REGISTERD_TO_HELPER;
    }
    auto &motor = this->m_motor_map[statePtr->header.id];
    motor->state().position = 1.0 * static_cast<double>(motor->m_direction) * motor->state().data.Pos / 16384.0 * 2.0 *
                              M_PI;
    motor->state().velocity = 1.0 * static_cast<double>(motor->m_direction) * motor->state().data.W / 128.0;
    motor->state().torque = 1.0 * static_cast<double>(motor->m_direction) * motor->state().data.T / 256.0;
    motor->state().temperature = 1.0 * static_cast<double>(motor->m_direction) * motor->state().data.Temp;

    return ESF_SUCCESS;
}

template class UnitreeA1Helper<UART>;
} // namespace base
} // namespace esf

#endif