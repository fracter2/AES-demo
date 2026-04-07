#pragma once

#include "common.h"
#include "key_shegule.h"

#include <string>
#include <vector>

namespace aes {
	constexpr void ShiftRows(Block& block) noexcept;
	constexpr void ShiftRowsInverse(Block& block) noexcept;
	constexpr void MixColumns(Block& block) noexcept;
	constexpr void MixColumnsInverse(Block& block) noexcept;
	constexpr void SubBytes(Block& block) noexcept;
	constexpr void SubBytesInverse(Block& block) noexcept;
	constexpr void AddRoundKey(Block& block, const RoundKey& key) noexcept;
	constexpr void ApplyPadding(std::vector<byte>& plaintext);
	constexpr void RemovePadding(std::vector<byte>& plaintext);

	template <std::size_t size>
	constexpr void EncryptBlock(Block& block, const std::array<RoundKey, size>& keys) noexcept
	{
		// First round only
		AddRoundKey(block, keys[0]);

		// Most rounds
		for (int i = 1; i < (keys.size() - 1); i++) {
			SubBytes(block);
			ShiftRows(block);
			MixColumns(block);
			AddRoundKey(block, keys[i]);
		}

		// Final round only
		SubBytes(block);
		ShiftRows(block);
		AddRoundKey(block, keys.back());
	}

	template <std::size_t size>
	constexpr void DecryptBlock(Block& block, const std::array<RoundKey, size>& keys) noexcept
	{
		// Final round only
		AddRoundKey(block, keys.back());
		ShiftRowsInverse(block);
		SubBytesInverse(block);

		// Most rounds
		for (int i = (keys.size() - 2); i > 0; i--) {
			AddRoundKey(block, keys[i]);
			MixColumnsInverse(block);
			ShiftRowsInverse(block);
			SubBytesInverse(block);
		}

		// First round only
		AddRoundKey(block, keys[0]);
	}

	// Get the ciphertext of the given byte sequence, using AES in ECB mode with PKCS7 Padding.
	template <std::ranges::contiguous_range T>
	constexpr std::vector<byte> EncryptRange(const T& plaintext, const SmallKey& key)
	{
		if (plaintext.empty()) throw;

		// Copy plaintext
		std::vector<byte> ciphertext(plaintext.size() * sizeof(plaintext.front()));
		std::copy(plaintext.begin(), plaintext.end(), ciphertext.begin());
		ciphertext.reserve(sizeof(Block));		// + Block bytes to give space for padding
		ApplyPadding(ciphertext);				// AES can only encrypt blocks of 128 bits, so we use PKCS7 padding to make it the right length.

		// Encryption loop
		const auto roundKeys = GetRoundKeys(key);
		const int blockCount = ciphertext.size() / sizeof(Block);			// TODO Convert to Block array (for clarity)
		Block* blocksPtr = std::bit_cast<Block*>(ciphertext.data());
		for (int i = 0; i < blockCount; i++) {
			Block& block = *(blocksPtr + i);
			EncryptBlock(block, roundKeys);
		}

		return ciphertext;
	}

	template<std::ranges::contiguous_range T>
	constexpr std::vector<byte> DecryptRange(const T& ciphertext, const SmallKey& key)
	{

		if (ciphertext.size() % sizeof(Block) != 0) throw;
		if (ciphertext.empty()) throw;

		// Copy ciphertext
		std::vector<byte> plaintext(ciphertext.size() * sizeof(ciphertext.front()));
		std::copy(ciphertext.begin(), ciphertext.end(), plaintext.begin());

		// Encryption loop
		const auto roundKeys = GetRoundKeys(key);
		const int blockCount = plaintext.size() / sizeof(Block);			// TODO Convert to Block array (for clarity)
		Block* blocksPtr = std::bit_cast<Block*>(plaintext.data());
		for (int i = 0; i < blockCount; i++) {
			Block& block = *(blocksPtr + i);
			DecryptBlock(block, roundKeys);
		}

		RemovePadding(plaintext);

		return plaintext;
	}
};

TEST_CASE("aes-EncryptDecryptRange") {
	// Test values verified using https://legacy.cryptool.org/en/cto/aes-step-by-step 
	SUBCASE("byte vectors") {
		const SmallKey key = { 0x2b, 0x7e, 0x15, 0x16, 0x28 , 0xae , 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
		const std::vector<byte> plaintext = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
		std::vector<byte> res = aes::EncryptRange(plaintext, key);
		CHECK(res.size() == sizeof(Block) * 2);
		res = aes::DecryptRange(res, key);
		CHECK(res.size() == plaintext.size());
		CHECK(res == plaintext);
	}
	SUBCASE("strings") {
		const SmallKey key = { 0x2b, 0x7e, 0x15, 0x16, 0x28 , 0xae , 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
		const std::u8string plaintext = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
		std::vector<byte> res = aes::EncryptRange(plaintext, key);
		CHECK(res.size() == sizeof(Block) * 2);
		res = aes::DecryptRange(res, key);
		CHECK(res.size() == plaintext.size());
		CHECK(std::equal(res.begin(), res.end(), plaintext.begin()));
	}
}


