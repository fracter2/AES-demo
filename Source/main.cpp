/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

// This project is an implementation of AES by Theodor Rydberg (github.com/fracter2).


#include "aes.h"
#include <print>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>


int main(int argc, char** argv)
{
	// Doctest boilerplate
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	int testRes = context.run();
	if (context.shouldExit())
		return testRes;

	// Demo
	const SmallKey key = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, };
	const std::string initialPlaintext = std::string("Introduction to Computer Security");
	std::println("Input plaintext: {}", initialPlaintext);

	// The encryption mode is ECB (block-by-block). Input is padded according to PKCS7 (for valid 16B blocks).
	std::vector<byte> res = aes::EncryptRange(initialPlaintext, key);
	std::println("After encryption: {}", res);

	res = aes::DecryptRange(res, key);
	std::println("Decrypted ciphertext: {}", res);

	std::string resStr = { std::bit_cast<char*>(res.data()) };
	std::println("Final output: {}", res);
	

	return testRes;
}



