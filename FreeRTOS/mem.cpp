#include "mem.hpp"

#include "FreeRTOS.h"

void *operator new(std::size_t size)
{
    return pvPortMalloc(size);
}

void operator delete(void *ptr) noexcept
{
    vPortFree(ptr);
    return;
}

namespace esf
{
void *malloc(size_t size)
{
    return pvPortMalloc(size);
}

void free(void *ptr)
{
    vPortFree(ptr);
    return;
}
} // namespace esf
