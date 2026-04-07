#pragma once

#include "connectivity/connectivity.hpp"

namespace esf
{
namespace base
{
struct Dt7Protocol {
    struct {
        uint16_t ch0 = 1024; // 通道 0
        uint16_t ch1 = 1024; // 通道 1
        uint16_t ch2 = 1024; // 通道 2
        uint16_t ch3 = 1024; // 通道 3
        uint8_t s1 = 0;      // 开关 1
        uint8_t s2 = 0;      // 开关 2
        uint16_t tw = 1024;  // 滚轮
    } rc;
    struct {
        int16_t x = 0;       // 鼠标 X 轴
        int16_t y = 0;       // 鼠标 Y 轴
        int16_t z = 0;       // 鼠标 Z 轴
        uint8_t press_l = 0; // 鼠标左键
        uint8_t press_r = 0; // 鼠标右键
    } mouse;
    struct {
        uint16_t w : 1 = 0;     // 键盘 W 键
        uint16_t s : 1 = 0;     // 键盘 S 键
        uint16_t a : 1 = 0;     // 键盘 A 键
        uint16_t d : 1 = 0;     // 键盘 D 键
        uint16_t q : 1 = 0;     // 键盘 Q 键
        uint16_t e : 1 = 0;     // 键盘 E 键
        uint16_t shift : 1 = 0; // 键盘 Shift 键
        uint16_t ctrl : 1 = 0;  // 键盘 Ctrl 键
        uint16_t r : 1 = 0;     // 键盘 R 键
        uint16_t f : 1 = 0;     // 键盘 F 键
        uint16_t g : 1 = 0;     // 键盘 G 键
        uint16_t z : 1 = 0;     // 键盘 Z 键
        uint16_t x : 1 = 0;     // 键盘 X 键
        uint16_t c : 1 = 0;     // 键盘 C 键
        uint16_t v : 1 = 0;     // 键盘 V 键
        uint16_t b : 1 = 0;     // 键盘 B 键
    } key;
};

/**
 * @brief 大疆遥控器 DT7 数据解析类，使用 UART 作为通信接口
 *
 * @tparam ConnectivityType 通信接口类型，必须实现 Connectivity 接口
 */
template <typename ConnectivityType> class Dt7 : public WithConnectivity<ConnectivityType> {
public:
    using ConnectivityMessageType = typename ConnectivityType::MessageReceive; // 消息类型定义
    /**
     * @brief 构造函数，接受一个 Connectivity 实例作为通信接口
     *
     * @param connectivity 通信接口实例
     */
    Dt7(ConnectivityType &connectivity);
    /**
     * @brief 析构函数，默认析构
     *
     */
    ~Dt7() = default;
    /**
     * @brief 初始化函数，调用通信接口的初始化函数
     *
     * @return Dt7& 返回当前对象的引用，便于链式调用
     */
    Dt7 &init();
    /**
     * @brief 获取解析后的 DT7 数据
     *
     * @return Dt7Protocol& 返回 DT7 数据结构的引用
     */
    Dt7Protocol &data();
    /**
     * @brief 解析接收到的 DT7 数据，并将结果存储在内部数据结构中
     *
     * @param msg 原始数据
     *
     * @return Dt7& 返回当前对象的引用，便于链式调用
     */
    Dt7 &decode(ConnectivityMessageType &msg);

private:
    Dt7Protocol m_receiveData; // 存储解析后的 DT7 数据

    EsfStatus m_status; // 状态码，参考 EsfStatus 定义
};

} // namespace base
} // namespace esf