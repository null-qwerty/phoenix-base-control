#pragma once

#include <atomic>

namespace esf
{
template <typename T>
class DoubleBuffer {
public:
    DoubleBuffer();
    ~DoubleBuffer() = default;
    T &read();
    T &write();
    void swap();

private:
    T m_buffer[2];
    std::atomic<int> m_readIndex = 0;
};
} // namespace esf