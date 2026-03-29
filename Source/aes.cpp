// aes.cpp

#include "aes.h"
#include "key_shegule.h"
#include "sbox.h"

namespace {

	void ShiftRows(Block& block) 
	{ 
		// NOTE First row (0 to 4) is skipped
		std::rotate(block.begin() + 4, block.begin() + 4 + 1, block.begin() + 8);
		std::rotate(block.begin() + 8, block.begin() + 8 + 2, block.begin() + 12);
		std::rotate(block.begin() + 12, block.begin() + 12 + 3, block.begin() + 16);
	}

	// -- Source implementation originaly taken from wiki page of Rijndael_MixColumns --
	constexpr Word GMixColumn(const Word& in) noexcept
	{
		Word b;			// is each element of the array 'a' multiplied by 2 in Rijndael's Galois field
		unsigned char h;
		for (int i = 0; i < 4; i++) {
								/* h is set to 0x01 if the high bit of in[c] is set, 0x00 otherwise */
			h = in[i] >> 7;		/* logical right shift, thus shifting in zeros */
			b[i] = in[i] << 1;	/* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
			b[i] ^= h * 0x1B;	/* Rijndael's Galois field */
		}

		Word r;										// in[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field
		r[0] = b[0] ^ in[3] ^ in[2] ^ b[1] ^ in[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
		r[1] = b[1] ^ in[0] ^ in[3] ^ b[2] ^ in[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
		r[2] = b[2] ^ in[1] ^ in[0] ^ b[3] ^ in[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
		r[3] = b[3] ^ in[2] ^ in[1] ^ b[0] ^ in[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
		return r;
	}
	// --

	void MixColumns(Block& block)
	{
		for (int i = 0; i < 4; i++) {
			Word column = { block[i], block[i + 4], block[i + 8], block[i + 12] };
			column = GMixColumn(column);
			block[i] = column[0];
			block[i+4] = column[1];
			block[i+8] = column[2];
			block[i+12] = column[3];
		}
	}

	void AddRoundKey(Block& block, const RoundKey& key)
	{
		block = XorBytes(block, key);
	}

	template <std::size_t size>
	constexpr void EncryptBlock(Block& block, const std::array<RoundKey, size>& keys) noexcept
	{
		// First round only
		AddRoundKey(block, keys[0]);

		// Most rounds
		for (int i = 1; i < (keys.size() - 1); i++) {
			block = SubBytes(block);
			ShiftRows(block);
			MixColumns(block);
			AddRoundKey(block, keys[i]);
			
		}

		// Final round only
		block = SubBytes(block);
		ShiftRows(block);
		AddRoundKey(block, keys.back());
	}

	constexpr void ApplyPKCS7Padding(std::u8string& ciphertext) noexcept
	{
		const int leftoverBytes = ciphertext.size() % sizeof(Block);
		const int paddingCount = sizeof(Block) - leftoverBytes;			// NOTE This will guarantee 1 to sizeof(block) bytes are padded
		const char8_t padChar = static_cast<char8_t>(paddingCount);		// NOTE The padded bytes are always just the pad length
		for (int i = 0; i < paddingCount; i++)
			ciphertext.push_back(padChar);
	}
}

constexpr std::u8string aes::Encrypt(std::u8string_view plaintext, const SmallKey& key) {
	// Copy plaintext
	std::u8string ciphertext{ plaintext };

	// AES can only encrypt blocks of 128 bits, so we use PKCS7 padding to make it the right length.
	ApplyPKCS7Padding(ciphertext);

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


TEST_CASE("aes-ShiftRows") {
	const Block rising = { 
		0_b, 1_b, 2_b, 3_b, 
		4_b, 5_b, 6_b, 7_b,
		8_b, 9_b, 10_b, 11_b,
		12_b, 13_b, 14_b, 15_b
	};
	const Block risingShifted = {
		0_b, 1_b, 2_b, 3_b,
		5_b, 6_b, 7_b, 4_b,
		10_b, 11_b, 8_b, 9_b,
		15_b, 12_b, 13_b, 14_b
	};

	Block temp = rising;
	ShiftRows(temp);
	CHECK(temp == risingShifted);
}

TEST_CASE("aes-GMixColumn") {	
	// -- Source test values originaly taken from wiki page of Rijndael_MixColumns --
	const Word test1 =		{ 99, 71, 162, 240 };
	const Word test1res =	{ 93, 224, 112, 187 };
	const Word test2 =		{ 242, 10, 34, 92 };
	const Word test2res =	{ 159, 220, 88, 157 };
	const Word test3 =		{ 1, 1, 1, 1 };
	const Word test3res =	{ 1, 1, 1, 1, };
	const Word test4 =		{ 198, 198, 198, 198 };
	const Word test4res =	{ 198, 198, 198, 198 };
	const Word test5 =		{ 212, 212, 212, 213 };
	const Word test5res =	{ 213, 213, 215, 214 };
	const Word test6 =		{ 45, 38, 49, 76 };
	const Word test6res =	{ 77, 126, 189, 248 };

	CHECK(GMixColumn(test1) == test1res);
	CHECK(GMixColumn(test2) == test2res);
	CHECK(GMixColumn(test3) == test3res);
	CHECK(GMixColumn(test4) == test4res);
	CHECK(GMixColumn(test5) == test5res);
	CHECK(GMixColumn(test6) == test6res);
}


TEST_CASE("aes-MixColumns") {

}


TEST_CASE("aes-AddRoundKey") {
	// TODO TEST CORRECT ADDING
}


TEST_CASE("aes-ApplyPKCS7Padding") {
	SUBCASE("Fill Remainder 1") {
		std::u8string test = std::u8string(sizeof(Block) - 1, 0_b);
		ApplyPKCS7Padding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test.back()	== 1);
	}

	SUBCASE("Fill Remainder 2") {
		std::u8string test = std::u8string(sizeof(Block) - 2, 0_b);
		ApplyPKCS7Padding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test.back()	== 2);
	}

	SUBCASE("Fill Single Byte") {
		std::u8string test = std::u8string(1, 0_b);
		ApplyPKCS7Padding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test[2]		== sizeof(Block) - 1);
		CHECK(test.back()	== sizeof(Block) - 1);
	}

	SUBCASE("Append Full Block") {
		std::u8string test = std::u8string(sizeof(Block), 0_b);
		ApplyPKCS7Padding(test);
		CHECK(test.size()				== 2 * sizeof(Block));
		CHECK(test.front()				== 0_b);
		CHECK(test[sizeof(Block) - 1]	== 0_b);
		CHECK(test[sizeof(Block)]		== sizeof(Block));
		CHECK(test.back()				== sizeof(Block));
	}

}

