// aes.cpp

#include "aes.h"

namespace {
	


	void SubBytes(Block& block) 
	{
		// TODO Substitue each byte using the S-box
	}

	void ShiftRows(Block& block) 
	{ 
		// TODO Shift each row LEFT by it's row index... so row0 zero times, row 1 once...
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




// TODO FIRST AES KEY SCHEGULE (key expansion) - creation of a "round key" (for each round)

// For each block of plaintext, we do multiple rounds of multiple steps.
// 
// First-round - Only step 4
// Final-round - Skip step 3

// Each round has these 4 steps

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