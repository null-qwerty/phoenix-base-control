#include "bmi088.hpp"
#include "sensor/bmi088_reg.hpp"
#include "thread.hpp"

namespace esf
{
namespace base
{
template <typename ConnectivityType>
BMI088<ConnectivityType>::BMI088(ConnectivityType &connectivity)
    : WithConnectivity<ConnectivityType>(connectivity)
{
}

template <typename ConnectivityType> BMI088<ConnectivityType> &BMI088<ConnectivityType>::init()
{
    // 加速度计复位，寄存器重置，重置完后加速度计默认为休眠状态
    _writeDataToAcc(BMI088_ACC_SOFTRESET_REG, BMI088_ACC_SOFTRESET_VALUE);
    HAL_Delay(1);
    // 陀螺仪复位，寄存器重置，重置完后陀螺仪默认为开启状态
    _writeDataToGyro(BMI088_GYRO_SOFTRESET_REG, BMI088_GYRO_SOFTRESET_VALUE);
    HAL_Delay(50);
    return _initAcc()._initGyro();
}

template <typename ConnectivityType>
BMI088<ConnectivityType> &BMI088<ConnectivityType>::setGyroCalib(float k_yaw,
                                                                 float k_pitch,
                                                                 float k_roll,
                                                                 float b_yaw,
                                                                 float b_pitch,
                                                                 float b_roll)
{
    m_gyroCalib.k_yaw = k_yaw;
    m_gyroCalib.k_pitch = k_pitch;
    m_gyroCalib.k_roll = k_roll;
    m_gyroCalib.b_yaw = b_yaw;
    m_gyroCalib.b_pitch = b_pitch;
    m_gyroCalib.b_roll = b_roll;
    return *this;
}

template <typename ConnectivityType> Bmi088Data &BMI088<ConnectivityType>::data()
{
    return m_data;
}

template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::readData()
{
    EsfStatus errcode = ESF_SUCCESS;
    this->_readAcc(); // 读加速度计
    if (m_acc_status != ESF_SUCCESS) {
        errcode = m_acc_status;
    }
    this->_readGyro(); // 读角速度计
    if (m_gyro_status != ESF_SUCCESS) {
        errcode = errcode == ESF_SUCCESS ? m_gyro_status : ESF_MULTIPLE_ERROR;
    }
    this->_readTemprature(); // 读温度

    if (m_temprature_status != ESF_SUCCESS) {
        errcode = errcode == ESF_SUCCESS ? m_temprature_status : ESF_MULTIPLE_ERROR;
    }
    this->_calibrateGyro(); // 矫正角速度
    return errcode;
}

template <typename ConnectivityType> BMI088<ConnectivityType> &BMI088<ConnectivityType>::_initAcc()
{
    _readDataFromAcc(BMI088_CHIP_ID_REG, &m_accChipId);
    _readDataFromAcc(BMI088_CHIP_ID_REG, &m_accChipId);
    // 加速度计默认为休眠状态，需要设置为激活状态
    _writeDataToAcc(BMI088_ACC_PWR_CFG_REG, BMI088_ACC_PWR_CFG_ACTIVE);
    HAL_Delay(10);
    // 设置加速度计量程为 ±3g，带宽为正常模式，输出数据速率为 1600Hz
    _writeDataToAcc(BMI088_ACC_RANGE_REG, BMI088_ACC_RANGE_3G);
    HAL_Delay(10);
    _writeDataToAcc(BMI008_ACC_CONF_REG, (1 << 7) | (BMI088_ACC_CONF_BWP_NORM << 4) | (BMI088_ACC_CONF_ODR_1600_Hz));
    // 设置加速度计为激活状态
    _writeDataToAcc(BMI088_ACC_PWR_CTRL_REG, BMI088_ACC_PWR_CTRL_ON);
    HAL_Delay(10);
    // 读取加速度计芯片 ID，正常情况下应该为 0x1E
    _readDataFromAcc(BMI088_CHIP_ID_REG, &m_accChipId);
    _readDataFromAcc(BMI088_CHIP_ID_REG, &m_accChipId);
    return *this;
}

template <typename ConnectivityType> BMI088<ConnectivityType> &BMI088<ConnectivityType>::_initGyro()
{
    _readDataFromGyro(BMI088_CHIP_ID_REG, &m_gyroChipId);
    _readDataFromGyro(BMI088_CHIP_ID_REG, &m_gyroChipId);
    // 设置陀螺仪量程为 ±2000°/s
    _writeDataToGyro(BMI088_GYRO_RANGE_REG, BMI088_GYRO_RANGE_2000_DEG_S);
    HAL_Delay(10);
    // 设置陀螺仪输出数据速率为 1000Hz，滤波器带宽为 116Hz
    _writeDataToGyro(BMI088_GYRO_BANDWIDTH_REG, BMI088_GYRO_ODR_1000Hz_BANDWIDTH_116Hz);
    HAL_Delay(10);
    // 读取陀螺仪芯片 ID，正常情况下应该为 0x0F
    _readDataFromGyro(BMI088_CHIP_ID_REG, &m_gyroChipId);
    _readDataFromGyro(BMI088_CHIP_ID_REG, &m_gyroChipId);
    return *this;
}

template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::_readAcc()
{
    /*
     * 和读取角速度一样，加速度数据也可一次性读取，但第一个字节是无效的。
     * 参考 BMI088 数据手册 6.1.2 节：
     * 6.1.2 SPI interface of accelerometer part
     *
     * In case of read operations of the acclerometer part, the requested data
     * is not sent immediately, but instead first a dummy byte is sent, and
     * after this dummy byte the actual request register content is transmitted.
     *
     * This means that - in contrast to the description in section 6.1.1 - a
     * single byte read operation requires to read 2 bytes in burst mode, of
     * which the first received byte can be discared, while the second byte
     * contains the desired data.
     *
     * the same applies to burst-read operations. For example, to read the
     * accelerometer values in SPI mode, the user has to read 7 bytes, starting
     * from address 0x12 (ACC data). From these bytes the user must discard the
     * first byte and finds the acceleration information in byte #2 - #7
     * (corresponding to the content of the addresses 0x12 - 0x17).
     *
     * The data bits are used as follows:
     * - Bit #0: Read/Write bit. When 0, the data SDI is written into the chip.
     *           When 1, the data SDO from the chip is read.
     * - Bit #1-7: Address AD(6:0).
     * - Bit #8-15:
     *     - When in write mode, these are the data SDI, which will be written
     *       into the address.
     *     - When in read mode, these bits contain unperdictable values, and the
     *       user has to read Bit #16-23 to get the actual data from the reading
     *       address.
     */
    m_acc_status = ESF_SUCCESS;
    m_acc_status = this->_readDataFromAcc(BMI088_ACC_OUT_REG, m_accBuf, 7);
    if (m_acc_status != ESF_SUCCESS) {
        return m_acc_status;
    }

    m_data.accel.x = static_cast<int16_t>((m_accBuf[2] << 8) | m_accBuf[1]) * BMI088_ACC_SEN;
    m_data.accel.y = static_cast<int16_t>((m_accBuf[4] << 8) | m_accBuf[3]) * BMI088_ACC_SEN;
    m_data.accel.z = static_cast<int16_t>((m_accBuf[6] << 8) | m_accBuf[5]) * BMI088_ACC_SEN;

    return m_acc_status;
}
template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::_readGyro()
{
    /*
     * 保持 CSB 为低电平，可以一次性读取所有的角速度数据，参考 BMI088
     * 数据手册 6.1.1 节：
     * 6.1.1 SPI interface of gyroscope part
     *
     * For single byte read as well as write operations, 16-bit protocols are
     * used. The SPI interface also supports multiple-byte read
     * operations(burst-read).
     *
     * The communication starts when the CSB (1 or 2) is pulled low by the SPI
     * master and stops when CSB (1 or 2) is pulled high. SCK is also controlled
     * by SPI master. SDI and SDO (1 or 2) are driven at the falling edge of SCK
     * and should be captured at the rising edge of SCK.
     *
     * the data bits are used as follows:
     * - Bit #0: Read/Write bit. When 0, the data SDI is weitten into
     *           the chip. When 1, the data SDO from the chip is read.
     * - Bit #1-7: Address AD(6:0).
     * - Bit #8-15: when in write mode, these are the data SDI, which will be
     *              written into the address. When in read mode, these are the
     *              data SDO, which are read from the address.
     *
     * Multiple read operations (burst-read) are possible by keeping CSB low and
     * continuing the data transfer (i.e. continuing to toggle SCK). Only the
     * first register address has to be written. Addresses are automatically
     * incremented after each read access as lang as CSB stays active low.
     */
    m_gyro_status = ESF_SUCCESS;
    m_gyro_status = this->_readDataFromGyro(BMI088_GYRO_OUT_REG, m_gyroBuf, 6);
    if (m_gyro_status != ESF_SUCCESS) {
        return m_gyro_status;
    }

    m_data.gyro.roll = static_cast<int16_t>((m_gyroBuf[1] << 8) | m_gyroBuf[0]) * BMI088_GYRO_SEN;
    m_data.gyro.pitch = static_cast<int16_t>((m_gyroBuf[3] << 8) | m_gyroBuf[2]) * BMI088_GYRO_SEN;
    m_data.gyro.yaw = static_cast<int16_t>((m_gyroBuf[5] << 8) | m_gyroBuf[4]) * BMI088_GYRO_SEN;

    return m_gyro_status;
}
template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::_readTemprature()
{
    // 同加速度计，第一个字节是无效的
    m_temprature_status = ESF_SUCCESS;
    m_temprature_status = this->_readDataFromAcc(BMI088_TEMPRATURE_OUT_REG, m_tempBuf, 3);
    if (m_temprature_status != ESF_SUCCESS) {
        return m_temprature_status;
    }

    int16_t temp = static_cast<int16_t>((m_tempBuf[1] << 3)) | (m_tempBuf[2] >> 5);
    if (temp > 1023) {
        temp -= 2048;
    }
    m_data.temperature = static_cast<float>(temp) * 0.125f + 23.0f;

    return m_temprature_status;
}

template <typename ConnectivityType> BMI088<ConnectivityType> &BMI088<ConnectivityType>::_calibrateGyro()
{
    if (m_gyro_status != ESF_SUCCESS || m_temprature_status != ESF_SUCCESS) {
        return *this;
    }
    m_data.gyro.yaw += m_gyroCalib.k_yaw * m_data.temperature + m_gyroCalib.b_yaw;
    m_data.gyro.pitch += m_gyroCalib.k_pitch * m_data.temperature + m_gyroCalib.b_pitch;
    m_data.gyro.roll += m_gyroCalib.k_roll * m_data.temperature + m_gyroCalib.b_roll;
    return *this;
}

template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::_writeDataToAcc(uint8_t reg, uint8_t data)
{
    ENABLE_ACC();
    auto errcode = _writeByte(reg, data);
    DISABLE_ACC();
    return errcode;
}
template <typename ConnectivityType> EsfStatus BMI088<ConnectivityType>::_writeDataToGyro(uint8_t reg, uint8_t data)
{
    ENABLE_GYRO();
    auto errcode = _writeByte(reg, data);
    DISABLE_GYRO();
    return errcode;
}

template <typename ConnectivityType>
EsfStatus BMI088<ConnectivityType>::_readDataFromAcc(uint8_t reg, uint8_t *data, size_t len)
{
    ENABLE_ACC();
    auto errcode = _readByte(reg, data, len);
    DISABLE_ACC();
    return errcode;
}
template <typename ConnectivityType>
EsfStatus BMI088<ConnectivityType>::_readDataFromGyro(uint8_t reg, uint8_t *data, size_t len)
{
    ENABLE_GYRO();
    auto errcode = _readByte(reg, data, len);
    DISABLE_GYRO();
    return errcode;
}

} // namespace base
} // namespace esf

#include "connectivity/spi.hpp"
#ifdef HAL_SPI_MODULE_ENABLED
/* SPI 通信下的读写函数 */
namespace esf
{
namespace base
{
template <> EsfStatus BMI088<SPI>::_readByte(uint8_t reg, uint8_t *buf, size_t len)
{
    uint8_t bmi088_read_send_reg = (reg | BMI088_READ);
    EsfStatus errcode = ESF_SUCCESS;
    ConnectivityMessageType send_frame = { .data = &bmi088_read_send_reg, .size = 1 };
    ConnectivityMessageType receive_frame = { .data = buf, .size = len };
    errcode = this->m_connectivity.sendMessage(send_frame);
    errcode = this->m_connectivity.receiveMessage(receive_frame);
    if (errcode != ESF_SUCCESS) {
        return errcode;
    }

    return errcode;
}

template <> EsfStatus BMI088<SPI>::_writeByte(uint8_t reg, uint8_t data)
{
    reg |= BMI088_WRITE;
    uint8_t bmi088_send_buf[2];
    bmi088_send_buf[0] = reg;
    bmi088_send_buf[1] = data;
    EsfStatus errcode = ESF_SUCCESS;
    ConnectivityMessageType send_frame = { .data = bmi088_send_buf, .size = 2 };
    errcode = this->m_connectivity.sendMessage(send_frame);
    if (errcode != ESF_SUCCESS) {
        return errcode;
    }
    return errcode;
}
template class BMI088<SPI>;

} // namespace base
} // namespace esf
#endif