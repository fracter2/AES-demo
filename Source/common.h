#pragma once

#include <doctest.h>

#include <stdexcept>
#include <array>
#include <algorithm>
#include <concepts>
#include <ranges>


using byte = unsigned char;
using Word = std::array<byte, 4>;
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

template<typename T>
concept IsByteArray = requires (T t, std::size_t n) {
    requires std::ranges::contiguous_range<T>;
    { t[n] } -> std::same_as<byte&>;
    { t.size() } -> std::same_as<std::size_t>;
};

static_assert(IsByteArray<Block>);


//
// ---- BYTE OPERATIONS ----
//

template <IsByteArray T>
[[nodiscard]] constexpr T RotBytes(T bytes) noexcept
{
    std::rotate(bytes.begin(), bytes.begin() + 1, bytes.end());
    return bytes;
}

template <IsByteArray T>
[[nodiscard]] constexpr T XorBytes(T lhs, const T& rhs) noexcept
{
    for (int i = 0; i < sizeof(lhs); i++) {
        lhs[i] ^= rhs[i];
    }
    return lhs;
}


//
// ---- TESTS ----
//

TEST_CASE("common-XorBytes") {
    const std::array<byte, 4> zero{ 0_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> one{ 1_b, 0_b, 0_b, 0_b };

    CHECK(XorBytes(zero, zero) == zero);
    CHECK(XorBytes(zero, one) == one);
    CHECK(XorBytes(one, zero) == one);
    CHECK(XorBytes(one, one) == zero);
}
