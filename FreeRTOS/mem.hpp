#pragma once

#include <cstddef>

void *operator new(std::size_t size);
void operator delete(void *ptr) noexcept;

namespace esf
{
void *malloc(size_t size);
void free(void *ptr);
} // namespace esf