#include "dt7.hpp"

namespace esf
{
namespace base
{
template <typename ConnectivityType>
Dt7<ConnectivityType>::Dt7(ConnectivityType &connectivity)
    : WithConnectivity<ConnectivityType>(connectivity)
{
}

template <typename ConnectivityType>
Dt7<ConnectivityType> &Dt7<ConnectivityType>::init()
{
    this->m_connectivity.init();
    return *this;
}

template <typename ConnectivityType>
Dt7Protocol &Dt7<ConnectivityType>::data()
{
    return m_receiveData;
}

template <typename ConnectivityType>
Dt7<ConnectivityType> &Dt7<ConnectivityType>::decode()
{
    UartMessage msg;
    this->m_status = this->m_connectivity.popFromReceiveQueue(msg);
    if (msg.size != 18) {
        return *this; // 数据长度不正确，返回当前对象
    }
    const uint8_t *databuffer = msg.data;
    // 解析遥控器数据
    m_receiveData.rc.ch0 = (databuffer[0] | (databuffer[1] << 8)) & 0x07FF;
    m_receiveData.rc.ch1 = ((databuffer[1] >> 3) | (databuffer[2] << 5)) & 0x07FF;
    m_receiveData.rc.ch2 = ((databuffer[2] >> 6) | (databuffer[3] << 2) | (databuffer[4] << 10)) & 0x07FF;
    m_receiveData.rc.ch3 = ((databuffer[4] >> 1) | (databuffer[5] << 7)) & 0x07FF;
    m_receiveData.rc.s1 = ((databuffer[5] >> 4) & 0x000C) >> 2;
    m_receiveData.rc.s2 = ((databuffer[5] >> 4) & 0x0003);
    m_receiveData.rc.tw = (databuffer[6] | (databuffer[7] << 8)) & 0x07FF;
    // 解析鼠标数据
    m_receiveData.mouse.x = databuffer[8] | (databuffer[9] << 8);
    m_receiveData.mouse.y = databuffer[10] | (databuffer[11] << 8);
    m_receiveData.mouse.z = databuffer[12] | (databuffer[13] << 8);
    m_receiveData.mouse.press_l = databuffer[14];
    m_receiveData.mouse.press_r = databuffer[15];
    // 解析键盘数据
    uint8_t *key_data = (uint8_t *)&m_receiveData.key;
    key_data[0] = databuffer[16];
    key_data[1] = databuffer[17];
    return *this;
}
} // namespace base
} // namespace esf