// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#include <cstdint>
#include "runtime/kernel/xdm.hpp"
#include "runtime/kernel/free_list.hpp"
#include "runtime/host/mutex.hpp"

Mutex GLOBAL_KERNEL_LOCK;

void DestroyKernelObject(KernelObject* obj) {
    obj->~KernelObject();
    GLOBAL_USER_HEAP.free(obj);
}

std::uint32_t GetKernelHandle(KernelObject* obj) {
    assert(obj != GetInvalidKernelObject());
    return GLOBAL_MEMORY.map_virtual(obj);
}

void DestroyKernelObject(uint32_t handle) {
    DestroyKernelObject(GetKernelObject(handle));
}

bool IsKernelObject(uint32_t handle) {
    return (handle & 0x80000000) != 0;
}

bool IsKernelObject(void* obj) {
    return IsKernelObject(GLOBAL_MEMORY.map_virtual(obj));
}

bool IsInvalidKernelObject(void* obj) {
    return obj == GetInvalidKernelObject();
}
