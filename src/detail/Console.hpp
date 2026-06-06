#pragma once

#include <format>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#endif

#include "Object.hpp"

#ifdef _WIN32
static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

inline void EnableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

inline void EnableUnicodeOutput() {
#ifdef _WIN32
    // Set console output code page to UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
}

inline void ShowConsoleCursor(bool showFlag)
{
	std::cout << ((showFlag) ? "\x1b[28m" : "\x1b[8m");
}

// x is the column, y is the row. The origin (0,0) is top-left.
inline void setCursorPosition(size_t x, size_t y)
{
	std::cout << "\x1b[" << y << ";" << x << "H";
}


inline const char* consoleForegroundColourCode(Colour colour) {
    colour = colour * 255;

    static char buffer[22];

    snprintf(buffer, sizeof(buffer), "\x1b[38;2;%d;%d;%dm", (int)colour.r, (int)colour.g, (int)colour.b);

    return buffer;
}

inline const char* consoleBackgroundColourCode(Colour colour) {
    colour = colour * 255;

    static char buffer[22];

    snprintf(buffer, sizeof(buffer), "\x1b[48;2;%d;%d;%dm", (int)colour.r, (int)colour.g, (int)colour.b);

    return buffer;
}

inline std::string cls() {
    return "\x1b[2J\x1b[H";
}
