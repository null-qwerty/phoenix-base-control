#pragma once

#include "main.h"

#pragma pack(1)
// clang-format off
typedef union
{
    int32_t L;
    uint8_t u8[4];
    uint16_t u16[2];
    uint32_t u32;
    float F;
} COMData32;

typedef struct {
    uint16_t start = 0xeefe; // 包头, 两个字节 0xfe 和 0xee，小端序下先低位再高位
    uint8_t id;              // 电机 id 可以为 0、1、2、0xBB，0xBB 代表向所有电机广播
    uint8_t reserved;        // 预留位
} COMHead;

typedef struct {
    uint8_t mode = 10;       // 电机运动模式，0(停转)，5(开环缓慢转动)，10(闭环伺服控制)
    uint8_t ModifyBit = 255; // 电机内部控制参数修改位，请设置为255
    uint8_t ReadBit;         // 电机内部控制参数发送位，可忽略
    uint8_t reserved;        // 预留位
    uint32_t Modify;         // 电机参数修改数据，可忽略
    int16_t T;               // 电机前馈力矩 T_ff * 256
    int16_t W;               // 电机速度 W_des * 128
    int32_t Pos;             // 电机位置 P_des * 16384 / 2PI
    int16_t K_P;             // 电机位置刚度 k_p * 2048
    int16_t K_W;             // 电机速度刚度 k_d * 1024
    uint8_t LowHzMotorCmdIndex; // 电机低频控制命令索引，可忽略
    uint8_t LowHzMotorCmdByte;  // 电机低频控制命令，可忽略
    uint32_t Res;            // 预留位，可忽略
} MasterComdV3;

typedef struct {
    uint8_t mode;      // 电机当前运动模式
    uint8_t ReadBit;   // 表示电机内部控制参数是否修改成功
    int8_t Temp;       // 电机当前平均温度
    uint8_t MError;    // 电机报错信息
    COMData32 Read;    // 读取电机内部控制参数，可忽略
    int16_t T;         // 当前电机输出力矩 * 256
    int16_t W;         // 当前电机实际转速 * 128
    float LW;          // 经过滤波的电机实际转速
    int16_t W2;        // 关节编码器预留，可忽略
    float LW2;         // 关节编码器预留，可忽略
    int16_t Acc;       // 当前电机转动加速度
    int16_t OutAcc;    // 关节编码器预留
    int32_t Pos;       // 当前电机角度位置 * 16384 / 2PI
    int32_t Pos2;      // 关节编码器预留
    int16_t gyro[3];   // 电机控制板上 IMU 在三轴上的角速度
                       // 乘 2000 / 2 ^ 15 * 2PI / 360 后换算为 rad/s
    int16_t acc[3];    // 电机控制板上 IMU 在三轴上的线速度
                       // 乘 8 * 9.80665 / 2 ^ 15 后换算为 m/s^2
    int16_t Fgyro[3];  // 足端 IMU 预留，可忽略
    int16_t Facc[3];   //
    int16_t Fmag[3];   //
    int8_t Ftemp;      // 足端传感器预留，可忽略
    uint16_t Force16;  // 足端力传感器高 16 位数据，可忽略
    uint8_t Force8;    // 足端力传感器低 8 位数据，可忽略
    uint8_t FError;    // 足端力传感器错误标识，可忽略
    uint8_t Res;       // 预留位
} ServoComdV3;

typedef struct {
    COMHead header;
    MasterComdV3 data;
    uint32_t crc;
} UnitreeA1CommendData; // 控制帧

typedef struct {
    COMHead header;
    ServoComdV3 data;
    uint32_t crc;
} UnitreeA1StateData;   // 反馈帧
// clang-format on
#pragma pack()