// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#pragma once
#include "dependencies/o1heap.h"
#include "runtime/host/mutex.hpp"

struct Heap
{
    Mutex mutex;
    O1HeapInstance* heap;

    Mutex physicalMutex;
    O1HeapInstance* physicalHeap;

    void init();

    void* alloc(size_t size);
    void* alloc_physical(size_t size, size_t alignment);
    void free(void* ptr);

    size_t size(void* ptr);

    template<typename T, typename... Args>
    T* alloc(Args&&... args)
    {
        T* obj = (T*)alloc(sizeof(T));
        new (obj) T(std::forward<Args>(args)...);
        return obj;
    }

    template<typename T, typename... Args>
    T* alloc_physical(Args&&... args)
    {
        T* obj = (T*)alloc_physical(sizeof(T), alignof(T));
        new (obj) T(std::forward<Args>(args)...);
        return obj;
    }
};

extern Heap GLOBAL_USER_HEAP;
