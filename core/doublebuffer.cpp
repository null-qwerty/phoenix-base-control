#include "doublebuffer.hpp"

namespace esf
{
template <typename T>
DoubleBuffer<T>::DoubleBuffer()
{
}

template <typename T>
T &DoubleBuffer<T>::read()
{
    return m_buffer[m_readIndex.load()];
}

template <typename T>
T &DoubleBuffer<T>::write()
{
    int writeIndex = 1 - m_readIndex.load();
    return m_buffer[writeIndex];
}

template <typename T>
void DoubleBuffer<T>::swap()
{
    m_readIndex.store(1 - m_readIndex.load());
}
} // namespace esf