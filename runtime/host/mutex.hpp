// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include <Windows.h>

struct Mutex : RTL_CRITICAL_SECTION {
    inline Mutex() {
        InitializeCriticalSection(this);
    }

    inline ~Mutex() {
        DeleteCriticalSection(this);
    }

    inline void lock() {
        EnterCriticalSection(this);
    }

    inline void unlock() {
        LeaveCriticalSection(this);
    }
};
