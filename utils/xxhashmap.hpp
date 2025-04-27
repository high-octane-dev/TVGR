// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include <cstdint>
#include <unordered_map>
#include <xxhash.h>

struct xxHash
{
    using is_avalanching = void;

    std::uint64_t operator()(XXH64_hash_t const& x) const noexcept
    {
        return x;
    }
};

// template<typename T> using xxHashMap = ankerl::unordered_dense::map<XXH64_hash_t, T, xxHash>;
template<typename T> using xxHashMap = std::unordered_map<XXH64_hash_t, T, xxHash>;