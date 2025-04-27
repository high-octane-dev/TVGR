// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#include <Windows.h>
#include "recompiled/ppc/ppc_context.h"
#include "runtime/kernel/memory.hpp"

Memory::Memory() {
    base = reinterpret_cast<std::uint8_t*>(VirtualAlloc((void*)0x100000000ull, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

    if (base == nullptr) {
        base = reinterpret_cast<std::uint8_t*>(VirtualAlloc(nullptr, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    }

    if (base == nullptr) {
        return;
    }

    DWORD oldProtect;
    VirtualProtect(base, 4096, PAGE_NOACCESS, &oldProtect);

    for (size_t i = 0; PPCFuncMappings[i].guest != 0; i++) {
        if (PPCFuncMappings[i].host != nullptr) {
            insert_function(PPCFuncMappings[i].guest, PPCFuncMappings[i].host);
        }
    }
}

extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return GLOBAL_MEMORY.translate(ptr);
}