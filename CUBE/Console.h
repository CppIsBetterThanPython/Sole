#pragma once

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "Object.h"

static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

inline void EnableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

inline void EnableUnicodeOutput() {
    // Set console output code page to UTF-8
    SetConsoleOutputCP(CP_UTF8);
}

inline void ShowConsoleCursor(bool showFlag)
{
    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

// x is the column, y is the row. The origin (0,0) is top-left.
inline void setCursorPosition(size_t x, size_t y)
{
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
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