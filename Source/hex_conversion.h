#pragma once

#include "common.h"
#include <sstream>
#include <iomanip>

// Code is based on https://tweex.net/post/c-anything-tofrom-a-hex-string/ by Jamie Dale


template <std::ranges::contiguous_range T>
std::string ToHex(const T& data)
{
    const byte* byteData = std::bit_cast<const byte*>(data.data());
    std::stringstream hexStringStream;

    hexStringStream << std::hex << std::setfill('0') << std::uppercase;
    for (size_t index = 0; index < data.size(); ++index)
        hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);
    return hexStringStream.str();
}

std::vector<byte> FromHex(const std::string& hex)
{
    std::vector<byte> result = std::vector<byte>(hex.size() / 2);

    std::stringstream hexStringStream; hexStringStream >> std::hex;
    for (int i = 0; i < result.size(); i++)
    {
        // Read out and convert the string two characters at a time
        const int strIndex = i * 2;
        const char tmpStr[3] = { hex[strIndex], hex[strIndex + 1], 0 };

        // Reset and fill the string stream
        hexStringStream.clear();
        hexStringStream.str(tmpStr);

        // Do the conversion
        int tmpValue = 0;
        hexStringStream >> tmpValue;
        result[i] = static_cast<byte>(tmpValue);
    }
    return result;
}

//
// ---- TESTS ----
//

TEST_CASE("hex_conversion") {
    const std::array<byte, 4> zero{ 0_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> one{ 1_b, 0_b, 0_b, 0_b };
    const std::array<byte, 4> max{ 255_b, 255_b, 255_b, 255_b };

    CHECK(ToHex(zero) == "00000000");
    CHECK(std::ranges::equal(zero, FromHex("00000000")));
    CHECK(ToHex(one) == "01000000");
    CHECK(std::ranges::equal(one, FromHex("01000000")));
    CHECK(ToHex(max) == "FFFFFFFF");
    CHECK(std::ranges::equal(max, FromHex("FFFFFFFF")));
}