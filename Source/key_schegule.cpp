#include "key_shegule.h"
#include "sbox.h"
#include <cassert>

namespace {

    // Words are 32bit (4byte) segments that make up the keys.
    using Word = std::array<byte, 4>;

    template<typename T>
    concept IsWordConvertable = requires (T t) {
        sizeof(T) % sizeof(Word) != 0;
    };

    template<typename T> requires IsWordConvertable<T>
    constexpr int WordCount()
    {
        return sizeof(T) / sizeof(Word);
    }

    template <typename T> requires IsWordConvertable<T>
    constexpr std::array<Word, WordCount<T>()> VarToWords(const T& var) noexcept        // TODO Consider renaming to "ToWords()" inside a namespace
    {
        std::array<Word, WordCount<T>()> r;
        const Word* const words = reinterpret_cast<const Word*>(&var);
        for (int i = 0; i < WordCount<T>(); i++) {
            r[i] = *(words + i);
        }

        // TODO TRY
        //const std::array<Word, WordCount<T>()>* r_ptr = reinterpret_cast<const std::array<Word, WordCount<T>()>>(&var);
        //return std::array<Word, WordCount<T>()> { *r_ptr };

        return r;
    }

    constexpr Word RotWord(Word word) noexcept
    {
        std::rotate(word.begin(), word.begin() + 1, word.end());
        return word;
    }

    constexpr Word SubWord(Word word) noexcept
    {
        return LookupSBoxDirectly<Word>(word);
    }

    constexpr Word XorWord(Word lhs, Word rhs) noexcept 
    {
        for (int i = 0; i < 4; i++) {
            lhs[i] ^= rhs[i];
        }
        return lhs;
    }

    template<typename T> 
    constexpr T DirectXOR(T lhs, T rhs) noexcept 
    {
        using _Ty = std::array<byte, sizeof(T)>;
        _Ty* lBytes = reinterpret_cast<_Ty*>(&lhs);
        _Ty* rBytes = reinterpret_cast<_Ty*>(&rhs);
        for (int i = 0; i < sizeof(T); i++) {
            (*lBytes)[i] ^= (*rBytes)[i];
        }
        return lhs;
    }

    TEST_CASE("Key-Schegule-DirectXOR") {
        CHECK(DirectXOR(0_b, 0_b) == 0_b);
        CHECK(DirectXOR(0_b, 1_b) == 1_b);
        CHECK(DirectXOR(1_b, 0_b) == 1_b);
        CHECK(DirectXOR(1_b, 1_b) == 0_b);

        CHECK(DirectXOR(2_b, 1_b) == 3_b);
        CHECK(DirectXOR(1_b, 2_b) == 3_b);
        CHECK(DirectXOR(3_b, 2_b) == 1_b);
        CHECK(DirectXOR(2_b, 3_b) == 1_b);
    }

    // -- Code taken from samiam.org/key-schegule.html --
    constexpr byte rcon(int in) noexcept                    // TODO Consider range-enforced variant (like byte)
    { 
        assert(in >= 0);
        
        byte c = 1;
        if (in == 0) {
            return 0;
        }

        while (in != 1) {
            byte b;
            b = c & 0x80;
            c <<= 1;
            if (b == 0x80) {
                c ^= 0x1b;
            }
            in--;
        }
        return c;
    }
    // --



    //
    constexpr Word KeyExpansionCore(Word prev, int roundIndex) noexcept
    {
        prev = SubWord(RotWord(prev));
        prev[0] ^= rcon(roundIndex);         // Since rcon always only affects the leftmost byte, this is simpler than making a whole Word for it.
        return prev;
    }
    //SubWord(RotWord(roundKW[i-1])), rcon(i/N)
}

template<typename _FromKey, int roundKeyCount>
constexpr std::array<RoundKey, roundKeyCount> CreateRoundKeys(const _FromKey& initKey) noexcept
{
    using rKeyArray = std::array<RoundKey, roundKeyCount>;
    using rWordArray = std::array<Word, WordCount<RoundKey>() * roundKeyCount>;
    rWordArray rWords;

    // The first words are always just the initial key
    const auto initKW = VarToWords(initKey);
    const int N = initKW.size();                        // Small = 4, Medium = 6, Large = 8
    for (int i = 0; i < N; i++) {
        rWords[i] = initKW[i];
    }

    // Main key schegule loop
    for (int i = N; i < rWords.size(); i++) {
        const int roundI = i / N;
        const Word& prev = rWords[i - 1];
        if (i % N == 0)                 { rWords[i] = KeyExpansionCore(prev, roundI); }
        else if (N > 6 && i % N == 4)   { rWords[i] = SubWord(prev); }                      // NOTE Only active for LargeKeys
        else                            { rWords[i] = prev; }

        const Word& keyOffsetW = rWords[i - N];
        rWords[i] = XorWord(keyOffsetW, rWords[i]);
    }

    return std::bit_cast<rKeyArray, rWordArray>(rWords);
}

std::array<RoundKey, roundKeysForSmallKey> GetRoundKeys(const SmallKey& key) noexcept
{
    return CreateRoundKeys<SmallKey, roundKeysForSmallKey>(key);
}

std::array<RoundKey, roundKeysForMediumKey> GetRoundKeys(const MediumKey& key) noexcept
{
    return CreateRoundKeys<MediumKey, roundKeysForMediumKey>(key);
}

std::array<RoundKey, roundKeysForLargeKey> GetRoundKeys(const LargeKey& key) noexcept
{
    return CreateRoundKeys<LargeKey, roundKeysForLargeKey>(key);
}