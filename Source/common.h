#pragma once

#include <stdexcept>
#include <array>
#include <algorithm>
#include <bit>
#include <doctest.h>

using byte = unsigned char;									// TODO CONSIDER MOVING BYTE UTIL TO DEDICATED byte FILE
using Block = std::array<byte, 16>;
using RoundKey = Block;
using SmallKey = Block;
using MediumKey = std::array<byte, 24>;
using LargeKey = std::array<byte, 32>;


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

template <typename T>
[[nodiscard]] constexpr std::array<byte, sizeof(T)> ToBytes(T data) noexcept
{
	return std::bit_cast<std::array<byte, sizeof(T)>>(data);
}
