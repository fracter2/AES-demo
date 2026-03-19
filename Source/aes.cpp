// aes.cpp

#include "aes.h"
#include "sbox.h"

namespace {

	void ShiftRows(Block& block) 
	{ 
		// NOTE First row (0 to 4) is skipped
		std::rotate(block.begin() + 4, block.begin() + 4 + 1, block.begin() + 8);
		std::rotate(block.begin() + 8, block.begin() + 8 + 2, block.begin() + 12);
		std::rotate(block.begin() + 12, block.begin() + 12 + 3, block.begin() + 16);
	}

	void MixColumns(Block& block)
	{
		// Take each column of four bytes
	}

	void AddRoundkey(Block& block, const Block& key)
	{
		// TODO block XOR key, on each byte
	}

	
}




// TODO FIRST AES KEY SCHEDULE (key expansion) - creation of a "round key" (for each round)

// For each block of plaintext, we do multiple rounds of multiple steps.
// 
// First-round - Only step 4
// Final-round - Skip step 3

// Each round has these 4 steps
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

// Step 1 - Sub Bytes
// Using an S-box lookup table
// Make sure to never substitute by itself or the complement of it self


// Step 2 Shift Rows
// Shift each row LEFT by it's row index... so row0 zero times, row 1 once...


// Step 3 Mix Columns
// matric multiplication.
// Skipped on final round.


// Step 4 Add Round Key
// bitwise XOR the block with the corresponding round key