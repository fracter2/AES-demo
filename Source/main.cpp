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

// This project is a "best practice" refactor of Liv and Eric's Space Invaders remake, which
// itself is based on the source code of "raylib [core] example - Basic window", as seen 
// above.
//
// Refactor by Theodor Rydberg, for the final asignment of the "Modern c++" course 2025-2026

#include <iostream>
#include "raylib.h"
#include "App.h"

#include <exception>
#include <print>

int main(void)
{
	try {
		App app{};
		while (!WindowShouldClose())
		{
			app.Update();
			app.Render();
		}
	}
	catch (const std::runtime_error& e) {
		std::print("A runtime error occurred: {}", e.what());
	}
	catch (const std::exception& e) {
		std::print("an unknown exception occurred: {}", e.what());
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
