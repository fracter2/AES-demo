#pragma once

#include <stdexcept>

using byte = unsigned char;

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