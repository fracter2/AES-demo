// aes.cpp

#include "aes.h"
#include "sbox.h"
#include "gmix.h"

constexpr void aes::ShiftRows(Block& block) noexcept
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

constexpr void aes::ShiftRowsInverse(Block& block) noexcept
{
	// NOTE Start at 1 since 0 doesn't do anything anyway
	for (int i = 1; i < 4; i++) {
		Word row = { block[i], block[i + 4], block[i + 8], block[i + 12] };
		std::rotate(row.begin(), row.begin() + (4 - i), row.end());
		block[i] = row[0];
		block[i + 4] = row[1];
		block[i + 8] = row[2];
		block[i + 12] = row[3];
	}

}

constexpr void aes::MixColumns(Block& block) noexcept
{
	for (int i = 0; i < 16; i += 4) {
		Word column = { block[i], block[i + 1], block[i + 2], block[i + 3] };
		column = GMixColumn(column);
		block[i] = column[0];
		block[i + 1] = column[1];
		block[i + 2] = column[2];
		block[i + 3] = column[3];
	}
}

constexpr void aes::MixColumnsInverse(Block& block) noexcept
{
	for (int i = 0; i < 16; i += 4) {
		Word column = { block[i], block[i + 1], block[i + 2], block[i + 3] };
		column = GMixColumnInverse(column);
		block[i] = column[0];
		block[i + 1] = column[1];
		block[i + 2] = column[2];
		block[i + 3] = column[3];
	}
}

constexpr void aes::SubBytes(Block& block) noexcept
{
	block = LookupSBoxRange(block);
}

constexpr void aes::SubBytesInverse(Block& block) noexcept
{
	block = LookupSBoxInvRange(block);
}

constexpr void aes::AddRoundKey(Block& block, const RoundKey& key) noexcept
{
	block = XorBytes(block, key);
}

// Applies padding to reach full Block intervals, using the PKCS7 protocol. Always increases size by at least 1 byte.
constexpr void aes::ApplyPadding(std::vector<byte>& plaintext)
{
	const int leftoverBytes = plaintext.size() % sizeof(Block);
	const byte paddingCount = static_cast<byte>(sizeof(Block) - leftoverBytes);			// NOTE This will guarantee 1 to sizeof(block) bytes are padded							
	for (int i = 0; i < paddingCount; i++)												// NOTE The padded bytes are always just the pad length
		plaintext.push_back(paddingCount);
}

// Removes padding by checking the last byte value, using the PKCS7 protocol. WANRNING, safely throws if it is not padded correctly.
constexpr void aes::RemovePadding(std::vector<byte>& plaintext)
{
	// TODO ADD MORE SPECIFIC EXCEPTIONS

	if (plaintext.empty()) throw;
	if (plaintext.size() % sizeof(Block) != 0) throw;

	// Make sure it can actually unpadd properly
	const int paddingCount = plaintext.back();
	if (plaintext.size() <= paddingCount) throw;
	if (sizeof(Block) < paddingCount) throw;

	// Each padding byte must be the very same
	for (int i = 1; i <= paddingCount; i++)
		if (plaintext[plaintext.size() - i] != paddingCount)
			throw;

	// Actually unpadd
	for (int i = 0; i < paddingCount; i++)
		plaintext.pop_back();
}


TEST_CASE("aes-ShiftRows") {
	const Block test1 = {
		0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47
	};
	const Block test1res = {
		0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee
	};

	Block temp = test1;
	aes::ShiftRows(temp);
	CHECK(temp == test1res);
}

TEST_CASE("aes-ShiftRowsInverse") {
	const Block test1 = {
		0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee
	};
	const Block test1res = {
		0x09, 0x08, 0x62, 0xbf, 0x6f, 0x28, 0xe3, 0x04, 0x2c, 0x74, 0x7f, 0xee, 0xda, 0x4a, 0x6a, 0x47
	};

	Block temp = test1;
	aes::ShiftRowsInverse(temp);
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
	aes::MixColumns(temp);
	CHECK(temp == test1res);
}

TEST_CASE("aes-MixColumnsInverse") {
	const Block test1 = {
		0x52, 0x9f, 0x16, 0xc2, 0x97, 0x86, 0x15, 0xca, 0xe0, 0x1a, 0xae, 0x54, 0xba, 0x1a, 0x26, 0x59
	};
	const Block test1res = {
		0x09, 0x28, 0x7f, 0x47, 0x6f, 0x74, 0x6a, 0xbf, 0x2c, 0x4a, 0x62, 0x04, 0xda, 0x08, 0xe3, 0xee
	};
	Block temp = test1;
	aes::MixColumnsInverse(temp);
	CHECK(temp == test1res);
}


TEST_CASE("aes-AddRoundKey") {
	// TODO TEST CORRECT ADDING
}


TEST_CASE("aes-ApplyRemovePKCS7Padding") {
	SUBCASE("Fill Remainder 1") {
		std::vector<byte> test = std::vector<byte>(sizeof(Block) - 1, 0_b);
		aes::ApplyPadding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test.back()	== 1);
		SUBCASE("Undo") {
			aes::RemovePadding(test);
			CHECK(test == std::vector<byte>(sizeof(Block) - 1, 0_b));
		}
	}

	SUBCASE("Fill Remainder 2") {
		std::vector<byte> test = std::vector<byte>(sizeof(Block) - 2, 0_b);
		aes::ApplyPadding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test.back()	== 2);
		SUBCASE("Undo") {
			aes::RemovePadding(test);
			CHECK(test == std::vector<byte>(sizeof(Block) - 2, 0_b));
		}
	}

	SUBCASE("Fill Single Byte") {
		std::vector<byte> test = std::vector<byte>(1, 0_b);
		aes::ApplyPadding(test);
		CHECK(test.size()	== sizeof(Block));
		CHECK(test.front()	== 0_b);
		CHECK(test[2]		== sizeof(Block) - 1);
		CHECK(test.back()	== sizeof(Block) - 1);
		SUBCASE("Undo") {
			aes::RemovePadding(test);
			CHECK(test == std::vector<byte>(1, 0_b));
		}
	}

	SUBCASE("Append Full Block") {
		std::vector<byte> test = std::vector<byte>(sizeof(Block), 0_b);
		aes::ApplyPadding(test);
		CHECK(test.size()				== 2 * sizeof(Block));
		CHECK(test.front()				== 0_b);
		CHECK(test[sizeof(Block) - 1]	== 0_b);
		CHECK(test[sizeof(Block)]		== sizeof(Block));
		CHECK(test.back()				== sizeof(Block));
		SUBCASE("Undo") {
			aes::RemovePadding(test);
			CHECK(test == std::vector<byte>(sizeof(Block), 0_b));
		}
	}

}


