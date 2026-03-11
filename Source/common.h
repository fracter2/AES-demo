#pragma once

#include <stdexcept>
#include <array>
#include <algorithm>

using byte = unsigned char;

static_assert(sizeof(byte) == 1);

consteval auto operator""_b(unsigned long long int value)
{
	if (value > 0xFFu)
		throw std::out_of_range{ "byte value out of range" };

	return static_cast<byte>(value);
}

constexpr auto to_number(byte b) noexcept
{
	return static_cast<unsigned char>(b);
}

constexpr byte bytemax = 255_b;


using Block = std::array<byte, 16>;

using KeySmall = std::array<byte, 16>;
using KeyMedium = std::array<byte, 24>;
using KeyLarge = std::array<byte, 32>;