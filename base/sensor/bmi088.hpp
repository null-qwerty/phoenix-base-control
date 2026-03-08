#pragma once

#include "bmi088_reg.hpp"

#include "connectivity/connectivity.hpp"
#include "core/status.hpp"
#include <cstdint>

namespace esf
{
namespace base
{
template <typename ConnectivityType>
class BMI088 : public WithConnectivity<ConnectivityType> {
public:
    struct Data {
        struct {
            float x = 0;
            float y = 0;
            float z = 0;
        } acc;
        struct {
            float yaw = 0;
            float pitch = 0;
            float roll = 0;
        } gyro;
        float temperature = 0;
    };
    using ConnectivityMessageType = typename ConnectivityType::Message; // 消息类型定义

    BMI088(ConnectivityType &connectivity);
    BMI088 &init();
    BMI088 &setGyroCalib(float k_yaw, float k_pitch, float k_roll, float b_yaw, float b_pitch, float b_roll);
    Data &data();
    EsfStatus readData();

private:
    Data m_data;

    uint8_t *m_accBuf, *m_gyroBuf, *m_tempBuf;
    uint8_t m_accChipId, m_gyroChipId;

    EsfStatus m_acc_status, m_gyro_status, m_temprature_status;

    struct {
        float k_yaw = 0;
        float k_pitch = 0;
        float k_roll = 0;
        float b_yaw = 0;
        float b_pitch = 0;
        float b_roll = 0;
    } m_gyroCalib;

    BMI088 &_initAcc();
    BMI088 &_initGyro();

    EsfStatus _readAcc();
    EsfStatus _readGyro();
    EsfStatus _readTemprature();

    EsfStatus _readByte(uint8_t reg, uint8_t *buf, size_t len);
    EsfStatus _writeByte(uint8_t reg, uint8_t data);

    EsfStatus _writeDataToAcc(uint8_t reg, uint8_t data);
    EsfStatus _writeDataToGyro(uint8_t reg, uint8_t data);
    EsfStatus _readDataFromAcc(uint8_t reg, uint8_t *buf, size_t len = 1);
    EsfStatus _readDataFromGyro(uint8_t reg, uint8_t *buf, size_t len = 1);

    BMI088 &_calibrateGyro();
};
} // namespace base
} // namespace esf