// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include "runtime/kernel/xdm.hpp"

#define CURRENT_THREAD_HANDLE uint32_t(-2)

struct GuestThreadContext
{
    PPCContext ppcContext{};
    uint8_t* thread = nullptr;

    GuestThreadContext(uint32_t cpuNumber);
    ~GuestThreadContext();
};

struct GuestThreadParams
{
    uint32_t function;
    uint32_t value;
    uint32_t flags;
};

struct GuestThreadHandle : KernelObject
{
    GuestThreadParams params;
    std::atomic<bool> suspended;
    std::thread thread;

    GuestThreadHandle(const GuestThreadParams& params);
    ~GuestThreadHandle() override;

    uint32_t Wait(uint32_t timeout) override;
};

struct GuestThread
{
    static uint32_t Start(const GuestThreadParams& params);
    static GuestThreadHandle* Start(const GuestThreadParams& params, uint32_t* threadId);

    static uint32_t GetCurrentThreadId();
    static void SetLastError(uint32_t error);

#ifdef _WIN32
    static void SetThreadName(uint32_t threadId, const char* name);
#endif
};
