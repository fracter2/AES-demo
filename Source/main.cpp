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
// It's my final assignment hand-in of the computer security course I am taking.

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
