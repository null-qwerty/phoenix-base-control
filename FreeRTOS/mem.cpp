#include "mem.hpp"

#include "FreeRTOS.h"

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
