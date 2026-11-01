#pragma once

// Minimal portable console helpers (the original version used Windows-only
// <conio.h> / <windows.h> calls directly).

#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#include <cstdlib>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace console
{
    inline void clear_screen()
    {
#ifdef _WIN32
        std::system("cls");
#else
        std::cout << "\033[2J\033[H" << std::flush;
#endif
    }

    inline void sleep_ms(int milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // Reads one key press without waiting for Enter and without echo.
    inline int read_key()
    {
#ifdef _WIN32
        return _getch();
#else
        termios old_settings{}, raw{};
        tcgetattr(STDIN_FILENO, &old_settings);
        raw = old_settings;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        int ch = std::getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
        return ch;
#endif
    }

    inline bool is_enter(int ch) { return ch == '\r' || ch == '\n'; }
    inline bool is_backspace(int ch) { return ch == '\b' || ch == 127; }
}
