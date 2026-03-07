#pragma once

#include "connectivity/uart.hpp"

namespace esf
{
namespace base
{
struct Dt7Protocol {
    struct {
        uint16_t ch0 = 1024;
        uint16_t ch1 = 1024;
        uint16_t ch2 = 1024;
        uint16_t ch3 = 1024;
        uint8_t s1 = 0;
        uint8_t s2 = 0;
        uint16_t tw = 1024;
    } rc;
    struct {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        uint8_t press_l = 0;
        uint8_t press_r = 0;
    } mouse;
    struct {
        uint16_t w : 1 = 0;
        uint16_t s : 1 = 0;
        uint16_t a : 1 = 0;
        uint16_t d : 1 = 0;
        uint16_t q : 1 = 0;
        uint16_t e : 1 = 0;
        uint16_t shift : 1 = 0;
        uint16_t ctrl : 1 = 0;
        uint16_t r : 1 = 0;
        uint16_t f : 1 = 0;
        uint16_t g : 1 = 0;
        uint16_t z : 1 = 0;
        uint16_t x : 1 = 0;
        uint16_t c : 1 = 0;
        uint16_t v : 1 = 0;
        uint16_t b : 1 = 0;
    } key;
};

class Dt7 {
public:
    Dt7(UART_HandleTypeDef &handle);
    ~Dt7() = default;
    Dt7 &init();
    Dt7Protocol &data();
    Dt7 &decode();

private:
    UART *m_uart;
    Dt7Protocol m_receiveData;

    EsfReturnType errCode;
};

} // namespace base
} // namespace esf