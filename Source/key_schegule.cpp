#include "key_shegule.h"

// Words are 32bit (4byte) segments that make up the keys. each round key is 4 words (128 bits/ 16 bytes)

using Word = std::array<byte, 4>;


void RotWord(Word& word) noexcept
{
    std::rotate(word.begin(), word.begin() + 1, word.end());
}

void SubWord(Word& word) noexcept
{
    // TODO use sbox to shenanigan this
}