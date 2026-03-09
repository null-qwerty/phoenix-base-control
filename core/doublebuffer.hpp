#pragma once

#include <atomic>

namespace esf
{
template <typename T>
class DoubleBuffer {
public:
    DoubleBuffer()
    {
    }
    ~DoubleBuffer() = default;
    T &read()
    {
        return m_buffer[m_readIndex.load()];
    }
    T &write()
    {
        int writeIndex = 1 - m_readIndex.load();
        return m_buffer[writeIndex];
    }
    void swap()
    {
        m_readIndex.store(1 - m_readIndex.load());
    }

private:
    T m_buffer[2];
    std::atomic<int> m_readIndex = 0;
};
} // namespace esf