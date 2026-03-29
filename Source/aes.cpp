// aes.cpp

#include "aes.h"
#include "key_shegule.h"
#include "sbox.h"

namespace {

	void ShiftRows(Block& block) 
	{ 
		// NOTE Start at 1 since 0 doesn't do anything anyway
		for (int i = 1; i < 4; i++) {
			Word row = { block[i], block[i + 4], block[i + 8], block[i + 12] };
			std::rotate(row.begin(), row.begin() + i, row.end());
			block[i] = row[0];
			block[i + 4] = row[1];
			block[i + 8] = row[2];
			block[i + 12] = row[3];
		}

	}

	// -- Source implementation originaly taken from wiki page of Rijndael_MixColumns --
	constexpr Word GMixColumn(const Word& in) noexcept
	{
		Word b;					// is each element of the input bytes multiplied by 2 in Rijndael's Galois field
		byte h;		// is high bit
		for (int i = 0; i < 4; i++) {
								/* h is set to 0x01 if the high bit of in[c] is set, 0x00 otherwise */
			h = in[i] >> 7;		/* logical right shift, thus shifting in zeros */
			b[i] = in[i] << 1;	/* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
			b[i] ^= h * 0x1B;	/* Rijndael's Galois field */
		}

		Word r;										// so, "b[n]" is n multiplied by 2, and "b[n] ^ in[n]" is n multiplied by 3 (in Rijndael's Galois field)
		r[0] = b[0] ^ in[3] ^ in[2] ^ b[1] ^ in[1]; /* 2 * in0 + in3 + in2 + 3 * in1 */
		r[1] = b[1] ^ in[0] ^ in[3] ^ b[2] ^ in[2]; /* 2 * in1 + in0 + in3 + 3 * in2 */
		r[2] = b[2] ^ in[1] ^ in[0] ^ b[3] ^ in[3]; /* 2 * in2 + in1 + in0 + 3 * in3 */
		r[3] = b[3] ^ in[2] ^ in[1] ^ b[0] ^ in[0]; /* 2 * in3 + in2 + in1 + 3 * in0 */
		return r;
	}
	// --

	void MixColumns(Block& block)
	{
		for (int i = 0; i < 16; i += 4) {
			Word column = { block[i], block[i + 1], block[i + 2], block[i + 3] };
			column = GMixColumn(column);
			block[i] = column[0];
			block[i+1] = column[1];
			block[i+2] = column[2];
			block[i+3] = column[3];
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

// Get the ciphertext of the given plaintext, using AES in ECB mode with PKCS7 Padding.
constexpr std::u8string aes::Encrypt(std::u8string_view plaintext, const SmallKey& key) {
	// Copy plaintext
	std::u8string ciphertext{ plaintext };

	// AES can only encrypt blocks of 128 bits, so we use PKCS7 padding to make it the right length.
	//ApplyPKCS7Padding(ciphertext);

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
	const Block test1 = {
		0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47
	};
	const Block test1res = {
		0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee
	};

	Block temp = test1;
	ShiftRows(temp);
	CHECK(temp == test1res);
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
	const Block test1 = {
		0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee
	};
	const Block test1res = {
		0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59
	};
	Block temp = test1;
	MixColumns(temp);
	CHECK(temp == test1res);
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

