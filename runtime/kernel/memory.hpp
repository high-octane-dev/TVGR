// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include <cstdint>
#include <cassert>
#include "recompiled/ppc/ppc_context.h"

struct Memory {
public:
    std::uint8_t* base = nullptr;
public:
    Memory();

    inline bool is_in_range(const void* host) const noexcept {
        return host >= base && host < (base + PPC_MEMORY_SIZE);
    }

    inline void* translate(size_t offset) const noexcept {
        if (offset != 0) {
            assert(offset < PPC_MEMORY_SIZE);
        }
        return base + offset;
    }

    inline uint32_t map_virtual(const void* host) const noexcept {
        if (host) {
            assert(is_in_range(host));
        }
        return static_cast<uint32_t>(static_cast<const uint8_t*>(host) - base);
    }

    inline PPCFunc* find_function(uint32_t guest) const noexcept {
        return PPC_LOOKUP_FUNC(base, guest);
    }

    inline void insert_function(uint32_t guest, PPCFunc* host) {
        PPC_LOOKUP_FUNC(base, guest) = host;
    }
};

extern Memory GLOBAL_MEMORY;