#pragma once

#include "common.h"

constexpr int roundKeysForSmallKey = 11;
constexpr int roundKeysForMediumKey = 13;
constexpr int roundKeysForLargeKey = 15;
std::array<RoundKey, roundKeysForSmallKey> GetRoundKeys(const SmallKey& key) noexcept;
std::array<RoundKey, roundKeysForMediumKey> GetRoundKeys(const MediumKey& key) noexcept;
std::array<RoundKey, roundKeysForLargeKey> GetRoundKeys(const LargeKey& key) noexcept;

