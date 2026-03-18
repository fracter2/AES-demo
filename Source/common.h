#pragma once

#include <stdexcept>
#include <array>
#include <algorithm>
#include <bit>
#include <doctest.h>
#include <concepts>
#include <ranges>

using byte = unsigned char;									// TODO CONSIDER MOVING BYTE UTIL TO DEDICATED byte FILE
using Block = std::array<byte, 16>;
using RoundKey = Block;
using SmallKey = Block;
using MediumKey = std::array<byte, 24>;
using LargeKey = std::array<byte, 32>;


consteval auto operator""_b(unsigned long long int value)
{
	if (value > 0xFFu)
		throw std::out_of_range{ "byte value out of range" };

	return static_cast<byte>(value);
}

constexpr byte bytemax = 255_b;

template<typename T>
concept IsByteArray = requires (T t, std::size_t n) {
    requires std::ranges::contiguous_range<T>;
    { t[n] } -> std::same_as<byte&>;
    { t.size() } -> std::same_as<std::size_t>;
};

static_assert(IsByteArray<Block>);


template <typename T>
[[nodiscard]] constexpr std::array<byte, sizeof(T)> ToBytes(T data) noexcept
{
    return std::bit_cast<std::array<byte, sizeof(T)>>(data);
}

template <typename T>
[[nodiscard]] constexpr std::array<byte, sizeof(T)>& AsByteRef(T& data) noexcept
{
    return *std::bit_cast<std::array<byte, sizeof(T)>*>(&data);
}

template <typename T>
[[nodiscard]] constexpr const std::array<byte, sizeof(T)>& AsByteRef(const T& data) noexcept
{
    return *std::bit_cast<std::array<byte, sizeof(T)>*>(&data);
}

template <typename T> requires IsByteArray<T>
[[nodiscard]] constexpr T RotBytes(T bytes) noexcept
{
    std::rotate(bytes.begin(), bytes.begin() + 1, bytes.end());
    return bytes;
}

template <typename T> requires IsByteArray<T> 
[[nodiscard]] constexpr T XorBytes(T lhs, const T& rhs) noexcept
{
    for (int i = 0; i < sizeof(lhs); i++) {
        lhs[i] ^= rhs[i];
    }
    return lhs;
}

TEST_CASE("common-XorBytes") {
    const std::array<byte, 4> zero{ 0_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> one{ 1_b, 0_b, 0_b, 0_b };

    CHECK(XorBytes(zero, zero) == zero);
    CHECK(XorBytes(zero, one) == one);
    CHECK(XorBytes(one, zero) == one);
    CHECK(XorBytes(one, one) == zero);
}

TEST_CASE("common-ToBytes") {
    const std::array<byte, 4> zero{ 0_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> one{ 1_b, 0_b, 0_b, 0_b };
    std::uint32_t uinteger{ 0u };

    CHECK(ToBytes(0u) == zero);
    CHECK(ToBytes(1u) == one);

    uinteger = 5u;
    CHECK(ToBytes(uinteger) == ToBytes(5u));
}

TEST_CASE("common-AsByteRef") {
    const std::array<byte, 4> zero{ 0_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> one{ 1_b, 0_b, 0_b, 0_b };
    
    CHECK(XorBytes(AsByteRef(0u), AsByteRef(0u)) == zero);      
    CHECK(ToBytes(1u) == one);

    // NOTE it maintains constness. TODO add test for constness?
    std::uint32_t uinteger{ 0u };
    AsByteRef(uinteger)[0] = 1_b;
    CHECK(uinteger == 1u);
}
