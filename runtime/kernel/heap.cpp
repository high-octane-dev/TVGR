// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#include <mutex>
#include "runtime/kernel/heap.hpp"
#include "runtime/kernel/memory.hpp"
#include "runtime/hook.hpp"

constexpr size_t RESERVED_BEGIN = 0x7FEA0000;
constexpr size_t RESERVED_END = 0xA0000000;

void Heap::init()
{
    heap = o1heapInit(GLOBAL_MEMORY.translate(0x20000), RESERVED_BEGIN - 0x20000);
    physicalHeap = o1heapInit(GLOBAL_MEMORY.translate(RESERVED_END), 0x100000000 - RESERVED_END);
}

void* Heap::alloc(size_t size)
{
    std::lock_guard lock(mutex);

    return o1heapAllocate(heap, std::max<size_t>(1, size));
}

void* Heap::alloc_physical(size_t size, size_t alignment)
{
    size = std::max<size_t>(1, size);
    alignment = alignment == 0 ? 0x1000 : std::max<size_t>(16, alignment);

    std::lock_guard lock(physicalMutex);

    void* ptr = o1heapAllocate(physicalHeap, size + alignment);
    size_t aligned = ((size_t)ptr + alignment) & ~(alignment - 1);

    *((void**)aligned - 1) = ptr;
    *((size_t*)aligned - 2) = size + O1HEAP_ALIGNMENT;

    return (void*)aligned;
}

void Heap::free(void* ptr)
{
    if (ptr >= physicalHeap)
    {
        std::lock_guard lock(physicalMutex);
        o1heapFree(physicalHeap, *((void**)ptr - 1));
    }
    else
    {
        std::lock_guard lock(mutex);
        o1heapFree(heap, ptr);
    }
}

size_t Heap::size(void* ptr)
{
    if (ptr)
        return *((size_t*)ptr - 2) - O1HEAP_ALIGNMENT; // relies on fragment header in o1heap.c

    return 0;
}

uint32_t RtlAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t size)
{
    void* ptr = GLOBAL_USER_HEAP.alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return GLOBAL_MEMORY.map_virtual(ptr);
}

uint32_t RtlReAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer, uint32_t size)
{
    void* ptr = GLOBAL_USER_HEAP.alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    if (memoryPointer != 0)
    {
        void* oldPtr = GLOBAL_MEMORY.translate(memoryPointer);
        memcpy(ptr, oldPtr, std::min<size_t>(size, GLOBAL_USER_HEAP.size(oldPtr)));
        GLOBAL_USER_HEAP.free(oldPtr);
    }

    assert(ptr);
    return GLOBAL_MEMORY.map_virtual(ptr);
}

uint32_t RtlFreeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    if (memoryPointer != NULL)
        GLOBAL_USER_HEAP.free(GLOBAL_MEMORY.translate(memoryPointer));

    return true;
}

uint32_t RtlSizeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    if (memoryPointer != NULL)
        return (uint32_t)GLOBAL_USER_HEAP.size(GLOBAL_MEMORY.translate(memoryPointer));

    return 0;
}

uint32_t XAllocMem(uint32_t size, uint32_t flags)
{
    void* ptr = (flags & 0x80000000) != 0 ?
        GLOBAL_USER_HEAP.alloc_physical(size, (1ull << ((flags >> 24) & 0xF))) :
        GLOBAL_USER_HEAP.alloc(size);

    if ((flags & 0x40000000) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return GLOBAL_MEMORY.map_virtual(ptr);
}

void XFreeMem(uint32_t baseAddress, uint32_t flags)
{
    if (baseAddress != NULL)
        GLOBAL_USER_HEAP.free(GLOBAL_MEMORY.translate(baseAddress));
}

// RRE-TODO: Find offsets for both RRE_GAME_TARGET_CARS1 and RRE_GAME_TARGET_CARS3
GuestFunctionStub(sub_8210FE88); // HeapCreate
// GuestFunctionStub(sub_82BD9250); // HeapDestroy

GuestFunctionHook(sub_8210F6D0, RtlSizeHeap);
GuestFunctionHook(sub_82110438, RtlAllocateHeap);
GuestFunctionHook(sub_82110D20, RtlFreeHeap);
GuestFunctionHook(sub_82111008, RtlReAllocateHeap);

GuestFunctionHook(sub_8210D7D0, XAllocMem);
GuestFunctionHook(sub_8210D868, XFreeMem);
