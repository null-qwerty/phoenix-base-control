#pragma once

#include <cstddef>
#include <cstdint>

namespace esf
{
using Byte = std::uint8_t;

struct Bytes {
    Byte *data;
    size_t size;
};
} // namespace esf