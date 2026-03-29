#pragma once

#include "common.h"
#include <string>
#include <string_view>

// TODO func for getting sized keys from arbituary key


// TODO encrypt funcs taking small(16bytes), medium(24bytes), or large(32bytes) keys. + plaintext stringview
// TODO Vice versa for decryption, returning... pointer to cipher text?

namespace aes {
	constexpr std::u8string Encrypt(std::u8string_view plaintext, const SmallKey& key);
	constexpr std::u8string Decrypt(std::u8string_view ciphertext, const SmallKey& key);
	constexpr void ApplyPKCS7Padding(std::u8string& plaintext) noexcept;
	constexpr void RemovePKCS7Padding(std::u8string& plaintext);
}


