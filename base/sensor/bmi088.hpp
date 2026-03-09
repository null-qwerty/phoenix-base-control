#pragma once

#include "bmi088_reg.hpp"

#include "connectivity/connectivity.hpp"
#include "core/status.hpp"
#include <cstdint>

namespace esf
{
namespace base
{
/**
 * @brief BMI088 数据格式
 *
 */
struct Bmi088Data {
    struct {
        float x = 0;
        float y = 0;
        float z = 0;
    } accel;
    struct {
        float yaw = 0;
        float pitch = 0;
        float roll = 0;
    } gyro;
    float temperature = 0;
};
/**
 * @brief BMI088 接口，用于驱动 BMI088
 *
 * @tparam ConnectivityType 通信方式，已实现 SPI
 *
 * @note 需要在 CubeMX 中提前定义片选针脚 `CS1_Accel_GPIO_Port`,`CS1_Accel_Pin`,`CS1_Gyro_GPIO_Port`,`CS1_Gyro_Pin`
 */
template <typename ConnectivityType>
class BMI088 : public WithConnectivity<ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::Message; // 消息类型定义

    /**
     * @brief 构造函数，创建一个 BMI088 实例
     *
     * @param connectivity 通信接口实例，只实现了 SPI
     */
    BMI088(ConnectivityType &connectivity);
    /**
     * @brief 初始化 BMI088
     *
     * @return BMI088& 返回自身引用，便于链式调用
     */
    BMI088 &init();
    /**
     * @brief 设置加速度计矫正参数
     *
     * @param k_yaw yaw 误差根据温度变化函数的斜率
     * @param k_pitch pitch 误差根据温度变化函数的斜率
     * @param k_roll roll 误差根据温度变化函数的斜率
     * @param b_yaw yaw 误差根据温度变化函数的截距
     * @param b_pitch pitch 误差根据温度变化函数的截距
     * @param b_roll poll 误差根据温度变化函数的截距
     * @return BMI088& 返回自身引用，便于链式调用
     *
     * @note 一种可行的标定方式：记录在不同温度下陀螺仪一段时间中的三轴角速度平均值，进行一次多项式拟合获取参数。
             建议在实际工作温度前后取值。

     */
    BMI088 &setGyroCalib(float k_yaw, float k_pitch, float k_roll, float b_yaw, float b_pitch, float b_roll);
    /**
     * @brief 获取数据
     *
     * @return Data& 传感器数据
     */
    Bmi088Data &data();
    /**
     * @brief 从传感器读取数据
     *
     * @return EsfStatus 成功返回 ESF_SUCCESS(0)，其他情况参考 core/status.hpp
     */
    EsfStatus readData();

private:
    Bmi088Data m_data; // 传感器数据

    uint8_t *m_accBuf, *m_gyroBuf, *m_tempBuf; // 通信消息缓冲区指针
    uint8_t m_accChipId, m_gyroChipId; // 片选 id

    EsfStatus m_acc_status, m_gyro_status, m_temprature_status; // 加速度计，角速度计和温度获取状态码

    struct {
        float k_yaw = 0;
        float k_pitch = 0;
        float k_roll = 0;
        float b_yaw = 0;
        float b_pitch = 0;
        float b_roll = 0;
    } m_gyroCalib; // 角速度计矫正参数

    BMI088 &_initAcc(); // 初始化加速度计
    BMI088 &_initGyro(); // 初始化角速度计

    EsfStatus _readAcc(); // 获取加速度计数据并解析
    EsfStatus _readGyro(); // 获取角速度计数据并解析
    EsfStatus _readTemprature(); // 获取温度数据并解析

    EsfStatus _readByte(uint8_t reg, uint8_t *buf, size_t len); // 读取寄存器值
    EsfStatus _writeByte(uint8_t reg, uint8_t data); // 向寄存器写值

    EsfStatus _writeDataToAcc(uint8_t reg, uint8_t data); // 向加速度计相关的寄存器写值
    EsfStatus _writeDataToGyro(uint8_t reg, uint8_t data); // 向角速度计相关的数据写值
    EsfStatus _readDataFromAcc(uint8_t reg, uint8_t *buf, size_t len = 1); // 从加速度计相关的寄存器读值
    EsfStatus _readDataFromGyro(uint8_t reg, uint8_t *buf, size_t len = 1); // 从角速度计相关的寄存器读值

    BMI088 &_calibrateGyro(); // 矫正角速度计数值
};
} // namespace base
} // namespace esf