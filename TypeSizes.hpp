#pragma once
#include <cstdint>
#include <cstddef>
#ifdef __EMSCRIPTEN__
using ProgramAddressType = size_t;
using NumberType = float;
using IntegerType = int32_t;
using UIntegerType = uint32_t;
#else
using ProgramAddressType = size_t;
using NumberType = double;
using IntegerType = int64_t;
using UIntegerType = uint64_t;

#endif