#pragma once

#include "core/status.hpp"
#include "motor/UnitreeA1protocol.hpp"
#include "motor/motor.hpp"

#include <map>

#include "crc.h"

namespace esf
{
namespace base
{

struct UnitreeA1State : public BasicMotorState, public UnitreeA1StateData {};
struct UnitreeA1Commend : public BasicMotorState, public UnitreeA1CommendData {
    double kp;
    double kw;
};

class UnitreeA1 : public Motor<UnitreeA1, UnitreeA1State, UnitreeA1Commend> {
public:
    UnitreeA1(uint32_t id, float reduration_ratio = 1.0f, Direction direction = Direction::CCW);
    ~UnitreeA1() = default;

    friend Motor;
    template <typename ConnectivityType> friend class UnitreeA1Helper;
};

template <typename ConnectivityType>
class UnitreeA1Helper : public MotorHelper<UnitreeA1Helper<ConnectivityType>, UnitreeA1, ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message;

    UnitreeA1Helper(ConnectivityType &connectivity)
        : MotorHelper<UnitreeA1Helper<ConnectivityType>, UnitreeA1, ConnectivityType>(connectivity)
    {
    }
    ~UnitreeA1Helper() = default;

    static uint32_t crc32(uint32_t *ptr, uint32_t len)
    {
#ifdef HAL_CRC_MODULE_ENABLED
        return HAL_CRC_Calculate(&hcrc, ptr, len);
#else
        uint32_t xbit = 0;
        uint32_t data = 0;
        uint32_t CRC32 = 0xFFFFFFFF;
        const uint32_t dwPolynomial = 0x04c11db7;
        for (uint32_t i = 0; i < len; i++) {
            xbit = 1 << 31;
            data = ptr[i];
            for (uint32_t bits = 0; bits < 32; bits++) {
                if (CRC32 & 0x80000000) {
                    CRC32 <<= 1;
                    CRC32 ^= dwPolynomial;
                } else
                    CRC32 <<= 1;
                if (data & xbit)
                    CRC32 ^= dwPolynomial;
                xbit >>= 1;
            }
        }
        return CRC32;
#endif
    }

    friend MotorHelper<UnitreeA1Helper<ConnectivityType>, UnitreeA1, ConnectivityType>;

private:
    ConnectivityMessageType m_receive_frame, m_send_frame;
    EsfStatus m_helper_status;

    EsfStatus _registerMotorImpl(UnitreeA1 &motor)
    {
        this->m_motor_map[motor.id()] = &motor;
        _sendOneMotorCommend(&motor); // 获取初始状态，方便设置软限位
        return ESF_SUCCESS;
    }
    EsfStatus _encodeMessageImpl();
    EsfStatus _decodeMessageImpl(ConnectivityMessageType &message);

    inline EsfStatus _sendOneMotorCommend(UnitreeA1 *motor);
};

} // namespace base
} // namespace esf
