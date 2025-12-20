#pragma once

#include <cstddef>

namespace esf
{
void *malloc(size_t size);
void free(void *ptr);
} // namespace esf