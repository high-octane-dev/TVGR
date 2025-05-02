// Copyright (C) hedge-dev 2025, Licensed via GPL3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html).
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <ntstatus.h>
#include "runtime/hook.hpp"
#include "runtime/kernel/xdm.hpp"
#include "runtime/xenon/guest_thread.hpp"
#include "runtime/xenon/xcpu_context.hpp"
#include "runtime/kernel/xam.hpp"
#include "runtime/kernel/heap.hpp"
#include "runtime/xaudio/xaudio.hpp"
#include "runtime/logger.hpp"

struct Event final : KernelObject, HostObject<XKEVENT> {
    bool manual_reset;
    std::atomic<bool> signaled;

    Event(XKEVENT* header)
        : manual_reset(!header->Type), signaled(!!header->SignalState)
    {
    }

    Event(bool manualReset, bool initialState)
        : manual_reset(manualReset), signaled(initialState)
    {
    }

    uint32_t Wait(uint32_t timeout) override
    {
        if (timeout == 0)
        {
            if (manual_reset)
            {
                if (!signaled)
                    return STATUS_TIMEOUT;
            }
            else
            {
                bool expected = true;
                if (!signaled.compare_exchange_strong(expected, false))
                    return STATUS_TIMEOUT;
            }
        }
        else if (timeout == INFINITE)
        {
            if (manual_reset)
            {
                signaled.wait(false);
            }
            else
            {
                while (true)
                {
                    bool expected = true;
                    if (signaled.compare_exchange_weak(expected, false))
                        break;

                    signaled.wait(expected);
                }
            }
        }
        else {
            // FIXME: Is this what we're supposed to do?
            if (manual_reset) {
                signaled.wait(false);
            }
            else {
                std::chrono::time_point start_time = std::chrono::system_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time).count() < timeout) {
                    bool expected = true;
                    if (signaled.compare_exchange_weak(expected, false)) {
                        return STATUS_SUCCESS;
                    }
                    signaled.wait(expected);
                }
                return STATUS_TIMEOUT;
            }
        }
        return STATUS_SUCCESS;
    }

    bool Set()
    {
        signaled = true;

        if (manual_reset)
            signaled.notify_all();
        else
            signaled.notify_one();

        return TRUE;
    }

    bool Reset()
    {
        signaled = false;
        return TRUE;
    }
};

static std::atomic<uint32_t> GLOBAL_KE_SET_EVENT_GENERATION;

struct Semaphore final : KernelObject, HostObject<XKSEMAPHORE> {
    std::atomic<uint32_t> count;
    uint32_t max_count;

    Semaphore(XKSEMAPHORE* semaphore)
        : count(semaphore->Header.SignalState), max_count(semaphore->Limit)
    {
    }

    Semaphore(uint32_t count, uint32_t maximumCount)
        : count(count), max_count(maximumCount)
    {
    }

    uint32_t Wait(uint32_t timeout) override
    {
        if (timeout == 0)
        {
            uint32_t currentCount = count.load();
            if (currentCount != 0)
            {
                if (count.compare_exchange_weak(currentCount, currentCount - 1))
                    return STATUS_SUCCESS;
            }

            return STATUS_TIMEOUT;
        }
        else if (timeout == INFINITE)
        {
            uint32_t currentCount;
            while (true)
            {
                currentCount = count.load();
                if (currentCount != 0)
                {
                    if (count.compare_exchange_weak(currentCount, currentCount - 1))
                        return STATUS_SUCCESS;
                }
                else
                {
                    count.wait(0);
                }
            }

            return STATUS_SUCCESS;
        }
        else
        {
            // FIXME: Is this what we're supposed to do?
            uint32_t currentCount;
            std::chrono::time_point start_time = std::chrono::system_clock::now();
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time).count() < timeout) {
                currentCount = count.load();
                if (currentCount != 0) {
                    if (count.compare_exchange_weak(currentCount, currentCount - 1)) {
                        return STATUS_SUCCESS;
                    }
                }
                else {
                    count.wait(0);
                }
            }

            return STATUS_TIMEOUT;
        }
    }

    void Release(uint32_t releaseCount, uint32_t* previousCount)
    {
        if (previousCount != nullptr)
            *previousCount = count;

        assert(count + releaseCount <= max_count);

        count += releaseCount;
        count.notify_all();
    }
};

std::uint32_t StubReturn0() { return 0; }
std::uint32_t StubReturn1() { return 1; }

std::uint32_t GuestTimeoutToMilliseconds(be<std::int64_t>* timeout) {
    return timeout ? (*timeout * -1) / 10000 : INFINITE;
}

uint32_t ExCreateThread(be<uint32_t>* handle, uint32_t stackSize, be<uint32_t>* threadId, uint32_t xApiThreadStartup, uint32_t startAddress, uint32_t startContext, uint32_t creationFlags)
{
    // LOGF_UTILITY("0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}", (intptr_t)handle, stackSize, (intptr_t)threadId, xApiThreadStartup, startAddress, startContext, creationFlags);

    uint32_t hostThreadId;

    *handle = GetKernelHandle(GuestThread::Start({ startAddress, startContext, creationFlags }, &hostThreadId));

    if (threadId != nullptr)
        *threadId = hostThreadId;

    return 0;
}

uint32_t ExGetXConfigSetting(uint16_t Category, uint16_t Setting, void* Buffer, uint16_t SizeOfBuffer, be<uint32_t>* RequiredSize) {
    uint32_t data[4]{};

    switch (Category)
    {
        // XCONFIG_SECURED_CATEGORY
    case 0x0002:
    {
        switch (Setting)
        {
            // XCONFIG_SECURED_AV_REGION
        case 0x0002:
            data[0] = ByteSwap(0x00001000); // USA/Canada
            break;

        default:
            return 1;
        }
    }

    case 0x0003:
    {
        switch (Setting)
        {
        case 0x0001: // XCONFIG_USER_TIME_ZONE_BIAS
        case 0x0002: // XCONFIG_USER_TIME_ZONE_STD_NAME
        case 0x0003: // XCONFIG_USER_TIME_ZONE_DLT_NAME
        case 0x0004: // XCONFIG_USER_TIME_ZONE_STD_DATE
        case 0x0005: // XCONFIG_USER_TIME_ZONE_DLT_DATE
        case 0x0006: // XCONFIG_USER_TIME_ZONE_STD_BIAS
        case 0x0007: // XCONFIG_USER_TIME_ZONE_DLT_BIAS
            data[0] = 0;
            break;

            // XCONFIG_USER_LANGUAGE
        case 0x0009:
            data[0] = ByteSwap(1); // XboxLanguage::English
            break;

            // XCONFIG_USER_VIDEO_FLAGS
        case 0x000A:
            data[0] = ByteSwap(0x00040000);
            break;

            // XCONFIG_USER_RETAIL_FLAGS
        case 0x000C:
            data[0] = ByteSwap(1);
            break;

            // XCONFIG_USER_COUNTRY
        case 0x000E:
            data[0] = ByteSwap(103);
            break;

        default:
            return 1;
        }
    }
    }

    *RequiredSize = 4;
    memcpy(Buffer, data, (std::min)(static_cast<size_t>(SizeOfBuffer), sizeof(data)));

    return 0;
}

void KeBugCheck() {
    __builtin_debugtrap();
}

void KeBugCheckEx() {
    __builtin_debugtrap();
}

uint32_t KeDelayExecutionThread(uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout) {
    // We don't do async file reads.
    if (Alertable)
        return STATUS_USER_APC;

    uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);

    Sleep(timeout);
    return STATUS_SUCCESS;
}

void KeAcquireSpinLockAtRaisedIrql(uint32_t* spinLock) {
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, GLOBAL_CPU_CONTEXT->r13.u32))
            break;

        std::this_thread::yield();
    }
}

void KeInitializeSemaphore(XKSEMAPHORE* semaphore, uint32_t count, uint32_t limit) {
    semaphore->Header.Type = 5;
    semaphore->Header.SignalState = count;
    semaphore->Limit = limit;

    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
}

uint64_t KeQueryPerformanceFrequency() {
    return 49875000;
}

void KeQuerySystemTime(be<uint64_t>* time) {
    constexpr int64_t FILETIME_EPOCH_DIFFERENCE = 116444736000000000LL;

    auto now = std::chrono::system_clock::now();
    auto timeSinceEpoch = now.time_since_epoch();

    int64_t currentTime100ns = std::chrono::duration_cast<std::chrono::duration<int64_t, std::ratio<1, 10000000>>>(timeSinceEpoch).count();
    currentTime100ns += FILETIME_EPOCH_DIFFERENCE;

    *time = currentTime100ns;
}

uint32_t KeReleaseSemaphore(XKSEMAPHORE* semaphore, uint32_t increment, uint32_t adjustment, uint32_t wait) {
    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
    object->Release(adjustment, nullptr);
    return STATUS_SUCCESS;
}

void KeReleaseSpinLockFromRaisedIrql(uint32_t* spinLock) {
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

bool KeResetEvent(XKEVENT* pEvent) {
    return QueryKernelObject<Event>(*pEvent)->Reset();
}

uint32_t KeResumeThread(GuestThreadHandle* object) {
    assert(object != GetKernelObject(CURRENT_THREAD_HANDLE));

    object->suspended = false;
    object->suspended.notify_all();
    return 0;
}

uint32_t KeSetAffinityThread(uint32_t Thread, uint32_t Affinity, be<uint32_t>* lpPreviousAffinity) {
    if (lpPreviousAffinity) {
        *lpPreviousAffinity = 2;
    }
    return 0;
}

void KeSetBasePriorityThread(GuestThreadHandle* hThread, int priority) {
    if (priority == 16) {
        priority = 15;
    }
    else if (priority == -16) {
        priority = -15;
    }

    SetThreadPriority(hThread == GetKernelObject(CURRENT_THREAD_HANDLE) ? GetCurrentThread() : hThread->thread.native_handle(), priority);
}

bool KeSetEvent(XKEVENT* pEvent, uint32_t Increment, bool Wait) {
    bool result = QueryKernelObject<Event>(*pEvent)->Set();

    ++GLOBAL_KE_SET_EVENT_GENERATION;
    GLOBAL_KE_SET_EVENT_GENERATION.notify_all();

    return result;
}

static std::vector<size_t> GLOBAL_TLS_FREE_INDICES;
static size_t GLOBAL_TLS_NEXT_INDEX = 0;
static Mutex GLOBAL_TLS_ALLOCATION_MUTEX;

uint32_t KeTlsAlloc() {
    std::lock_guard<Mutex> lock(GLOBAL_TLS_ALLOCATION_MUTEX);
    if (!GLOBAL_TLS_FREE_INDICES.empty())
    {
        size_t index = GLOBAL_TLS_FREE_INDICES.back();
        GLOBAL_TLS_FREE_INDICES.pop_back();
        return index;
    }

    return GLOBAL_TLS_NEXT_INDEX++;
}

uint32_t KeTlsFree(uint32_t dwTlsIndex) {
    std::lock_guard<Mutex> lock(GLOBAL_TLS_ALLOCATION_MUTEX);
    GLOBAL_TLS_FREE_INDICES.push_back(dwTlsIndex);
    return TRUE;
}


static uint32_t& KeTlsGetValueRef(size_t index)
{
    // Having this a global thread_local variable
    // for some reason crashes on boot in debug builds.
    thread_local std::vector<uint32_t> s_tlsValues;

    if (s_tlsValues.size() <= index)
    {
        s_tlsValues.resize(index + 1, 0);
    }

    return s_tlsValues[index];
}

uint32_t KeTlsGetValue(uint32_t dwTlsIndex)
{
    return KeTlsGetValueRef(dwTlsIndex);
}

uint32_t KeTlsSetValue(uint32_t dwTlsIndex, uint32_t lpTlsValue)
{
    KeTlsGetValueRef(dwTlsIndex) = lpTlsValue;
    return TRUE;
}


uint32_t KeWaitForMultipleObjects(uint32_t Count, xpointer<XDISPATCHER_HEADER>* Objects, uint32_t WaitType, uint32_t WaitReason, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout)
{
    // FIXME: This function is only accounting for events.
    const uint64_t timeout = GuestTimeoutToMilliseconds(Timeout);

    if (WaitType == 0) // Wait all
    {
        for (size_t i = 0; i < Count; i++)
            QueryKernelObject<Event>(*Objects[i])->Wait(timeout);
    }
    else
    {
        thread_local std::vector<Event*> s_events;
        s_events.resize(Count);

        for (size_t i = 0; i < Count; i++)
            s_events[i] = QueryKernelObject<Event>(*Objects[i]);

        while (true)
        {
            uint32_t generation = GLOBAL_KE_SET_EVENT_GENERATION.load();

            for (size_t i = 0; i < Count; i++)
            {
                if (s_events[i]->Wait(0) == STATUS_SUCCESS)
                {
                    return STATUS_WAIT_0 + i;
                }
            }

            GLOBAL_KE_SET_EVENT_GENERATION.wait(generation);
        }
    }

    return STATUS_SUCCESS;
}

uint32_t KeWaitForSingleObject(XDISPATCHER_HEADER* Object, uint32_t WaitReason, uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout)
{
    const uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);

    switch (Object->Type)
    {
    case 0:
    case 1:
        QueryKernelObject<Event>(*Object)->Wait(timeout);
        break;

    case 5:
        QueryKernelObject<Semaphore>(*Object)->Wait(timeout);
        break;

    default:
        assert(false && "Unrecognized kernel object type.");
        return STATUS_TIMEOUT;
    }

    return STATUS_SUCCESS;
}

void KfAcquireSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, GLOBAL_CPU_CONTEXT->r13.u32))
            break;

        std::this_thread::yield();
    }
}
void KfReleaseSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

uint32_t MmAllocatePhysicalMemoryEx
(
    uint32_t flags,
    uint32_t size,
    uint32_t protect,
    uint32_t minAddress,
    uint32_t maxAddress,
    uint32_t alignment
)
{
    // LOGF_UTILITY("0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}", flags, size, protect, minAddress, maxAddress, alignment);
    return GLOBAL_MEMORY.map_virtual(GLOBAL_USER_HEAP.alloc_physical(size, alignment));
}

void MmFreePhysicalMemory(uint32_t type, uint32_t guestAddress) {
    if (guestAddress != NULL) {
        GLOBAL_USER_HEAP.free(GLOBAL_MEMORY.translate(guestAddress));
    }
}

uint32_t MmGetPhysicalAddress(uint32_t address) {
    // LOGF_UTILITY("0x{:x}", address);
    return address;
}

uint32_t MmQueryAddressProtect(uint32_t guestAddress) {
    return PAGE_READWRITE;
}

uint32_t NtClose(uint32_t handle) {
    if (handle == GUEST_INVALID_HANDLE_VALUE)
        return 0xFFFFFFFF;

    if (IsKernelObject(handle))
    {
        DestroyKernelObject(handle);
        return 0;
    }
    else
    {
        assert(false && "Unrecognized kernel object.");
        return 0xFFFFFFFF;
    }
}


uint32_t NtCreateEvent(be<uint32_t>* handle, void* objAttributes, uint32_t eventType, uint32_t initialState) {
    *handle = GetKernelHandle(CreateKernelObject<Event>(!eventType, !!initialState));
    return 0;
}

uint32_t NtCreateFile(be<uint32_t>* FileHandle, uint32_t DesiredAccess, XOBJECT_ATTRIBUTES* Attributes, XIO_STATUS_BLOCK* IoStatusBlock, uint64_t* AllocationSize, uint32_t FileAttributes, uint32_t ShareAccess, uint32_t CreateDisposition, uint32_t CreateOptions) {
    logger::log_format("[NtCreateFile] WARN: Stub called!");
    return 0;
}

void NtOpenFile() {
    logger::log_format("[NtOpenFile] WARN: Stub called!");
}

void NtQueryDirectoryFile() {
    logger::log_format("[NtQueryDirectoryFile] WARN: Stub called!");
}

void NtQueryFullAttributesFile() {
    logger::log_format("[NtQueryFullAttributesFile] WARN: Stub called!");
}

void NtQueryInformationFile() {
    logger::log_format("[NtQueryInformationFile] WARN: Stub called!");
}

void NtQueryVolumeInformationFile() {
    logger::log_format("[NtQueryVolumeInformationFile] WARN: Stub called!");
}

void NtReadFile() {
    logger::log_format("[NtReadFile] WARN: Stub called!");
}

void NtSetInformationFile() {
    logger::log_format("[NtSetInformationFile] WARN: Stub called!");
}

uint32_t NtResumeThread(GuestThreadHandle* hThread, uint32_t* suspendCount) {
    assert(hThread != GetKernelObject(CURRENT_THREAD_HANDLE));

    hThread->suspended = false;
    hThread->suspended.notify_all();

    return S_OK;
}

uint32_t NtSetEvent(Event* handle, uint32_t* previousState) {
    handle->Set();
    return 0;
}

uint32_t NtSuspendThread(GuestThreadHandle* hThread, uint32_t* suspendCount) {
    assert(hThread != GetKernelObject(CURRENT_THREAD_HANDLE) && hThread->thread.get_id() == std::this_thread::get_id());

    hThread->suspended = true;
    hThread->suspended.wait(true);

    return S_OK;
}

uint32_t NtWaitForSingleObjectEx(uint32_t Handle, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout) {
    uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    
    if (IsKernelObject(Handle))
    {
        return GetKernelObject(Handle)->Wait(timeout);
    }
    else {
        assert(false && "Unrecognized handle value.");
    }

    return STATUS_TIMEOUT;
}

uint32_t ObReferenceObjectByHandle(uint32_t handle, uint32_t objectType, be<uint32_t>* object) {
    *object = handle;
    return 0;
}


void RtlEnterCriticalSection(XRTL_CRITICAL_SECTION* cs) {
    uint32_t thisThread = GLOBAL_CPU_CONTEXT->r13.u32;
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    while (true)
    {
        uint32_t previousOwner = 0;

        if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
        {
            cs->RecursionCount++;
            return;
        }

        owningThread.wait(previousOwner);
    }
}

void RtlInitAnsiString(XANSI_STRING* destination, char* source) {
    const uint16_t length = source ? (uint16_t)strlen(source) : 0;
    destination->Length = length;
    destination->MaximumLength = length + 1;
    destination->Buffer = source;
}

uint32_t RtlInitializeCriticalSection(XRTL_CRITICAL_SECTION* cs) {
    cs->Header.Absolute = 0;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;

    return 0;
}

void RtlLeaveCriticalSection(XRTL_CRITICAL_SECTION* cs) {
    cs->RecursionCount--;

    if (cs->RecursionCount != 0)
        return;

    std::atomic_ref owningThread(cs->OwningThread);
    owningThread.store(0);
    owningThread.notify_one();
}

uint32_t RtlMultiByteToUnicodeN(be<uint16_t>* UnicodeString, uint32_t MaxBytesInUnicodeString, be<uint32_t>* BytesInUnicodeString, const char* MultiByteString, uint32_t BytesInMultiByteString) {
    uint32_t length = (std::min)(MaxBytesInUnicodeString / 2, BytesInMultiByteString);

    for (size_t i = 0; i < length; i++)
        UnicodeString[i] = MultiByteString[i];

    if (BytesInUnicodeString != nullptr)
        *BytesInUnicodeString = length * 2;

    return STATUS_SUCCESS;
}


uint32_t RtlUnicodeToMultiByteN(char* MultiByteString, uint32_t MaxBytesInMultiByteString, be<uint32_t>* BytesInMultiByteString, const be<uint16_t>* UnicodeString, uint32_t BytesInUnicodeString) {
    const auto reqSize = BytesInUnicodeString / sizeof(uint16_t);

    if (BytesInMultiByteString)
        *BytesInMultiByteString = reqSize;

    if (reqSize > MaxBytesInMultiByteString)
        return STATUS_FAIL_CHECK;

    for (size_t i = 0; i < reqSize; i++)
    {
        const auto c = UnicodeString[i].get();

        MultiByteString[i] = c < 256 ? c : '?';
    }

    return STATUS_SUCCESS;
}

bool VdPersistDisplay(uint32_t a1, uint32_t* a2) {
    *a2 = NULL;
    return false;
}

void VdQueryVideoMode(XVIDEO_MODE* vm) {
    memset(vm, 0, sizeof(XVIDEO_MODE));
    vm->DisplayWidth = 1280;
    vm->DisplayHeight = 720;
    vm->IsInterlaced = false;
    vm->IsWidescreen = true;
    vm->IsHighDefinition = true;
    vm->RefreshRate = 0x42700000;
    vm->VideoStandard = 1;
    vm->Unknown4A = 0x4A;
    vm->Unknown01 = 0x01;
}

uint32_t XAudioGetVoiceCategoryVolumeChangeMask(uint32_t Driver, be<uint32_t>* Mask) {
    *Mask = 0;
    return 0;
}

uint32_t XGetGameRegion() {
    return 0x03FF;
}

uint32_t XGetLanguage() {
    return 1; // XboxLanguage::English
}

uint32_t XMsgInProcessCall(uint32_t app, uint32_t message, be<uint32_t>* param1, be<uint32_t>* param2) {
    if (message == 0x7001B) {
        std::uint32_t* ptr = reinterpret_cast<std::uint32_t*>(GLOBAL_MEMORY.translate(param1[1]));
        ptr[0] = 0;
        ptr[1] = 0;
    }

    return 0;
}


GuestFunctionStub(__imp__DbgBreakPoint);
GuestFunctionStub(__imp__DbgPrint);
GuestFunctionHook(__imp__ExCreateThread, ExCreateThread);
GuestFunctionHook(__imp__ExGetXConfigSetting, ExGetXConfigSetting);
GuestFunctionStub(__imp__ExRegisterTitleTerminateNotification);
GuestFunctionStub(__imp__ExTerminateThread);
GuestFunctionHook(__imp__FscSetCacheElementCount, StubReturn0);
GuestFunctionStub(__imp__HalReturnToFirmware);
GuestFunctionStub(__imp__IoDismountVolumeByFileHandle); // TVG
GuestFunctionHook(__imp__KeAcquireSpinLockAtRaisedIrql, KeAcquireSpinLockAtRaisedIrql);
GuestFunctionHook(__imp__KeBugCheck, KeBugCheck);
GuestFunctionHook(__imp__KeBugCheckEx, KeBugCheckEx);
GuestFunctionHook(__imp__KeDelayExecutionThread, KeDelayExecutionThread);
GuestFunctionStub(__imp__KeEnableFpuExceptions);
GuestFunctionStub(__imp__KeEnterCriticalRegion);
GuestFunctionHook(__imp__KeGetCurrentProcessType, StubReturn1);
GuestFunctionHook(__imp__KeInitializeSemaphore, KeInitializeSemaphore);
GuestFunctionStub(__imp__KeLeaveCriticalRegion);
GuestFunctionStub(__imp__KeLockL2);
GuestFunctionHook(__imp__KeQueryPerformanceFrequency, KeQueryPerformanceFrequency);
GuestFunctionHook(__imp__KeQuerySystemTime, KeQuerySystemTime);
GuestFunctionHook(__imp__KeRaiseIrqlToDpcLevel, StubReturn0);
GuestFunctionHook(__imp__KeReleaseSemaphore, KeReleaseSemaphore);
GuestFunctionHook(__imp__KeReleaseSpinLockFromRaisedIrql, KeReleaseSpinLockFromRaisedIrql);
GuestFunctionHook(__imp__KeResetEvent, KeResetEvent);
GuestFunctionHook(__imp__KeResumeThread, KeResumeThread);
GuestFunctionHook(__imp__KeSetAffinityThread, KeSetAffinityThread);
GuestFunctionHook(__imp__KeSetBasePriorityThread, KeSetBasePriorityThread);
GuestFunctionHook(__imp__KeSetEvent, KeSetEvent);
GuestFunctionHook(__imp__KeTlsAlloc, KeTlsAlloc);
GuestFunctionHook(__imp__KeTlsFree, KeTlsFree);
GuestFunctionHook(__imp__KeTlsGetValue, KeTlsGetValue);
GuestFunctionHook(__imp__KeTlsSetValue, KeTlsSetValue);
GuestFunctionStub(__imp__KeUnlockL2);
GuestFunctionHook(__imp__KeWaitForMultipleObjects, KeWaitForMultipleObjects);
GuestFunctionHook(__imp__KeWaitForSingleObject, KeWaitForSingleObject);
GuestFunctionHook(__imp__KfAcquireSpinLock, KfAcquireSpinLock);
GuestFunctionStub(__imp__KfLowerIrql);
GuestFunctionHook(__imp__KfReleaseSpinLock, KfReleaseSpinLock);
GuestFunctionHook(__imp__KiApcNormalRoutineNop, StubReturn0);
GuestFunctionHook(__imp__MmAllocatePhysicalMemoryEx, MmAllocatePhysicalMemoryEx);
GuestFunctionHook(__imp__MmFreePhysicalMemory, MmFreePhysicalMemory);
GuestFunctionHook(__imp__MmGetPhysicalAddress, MmGetPhysicalAddress);
GuestFunctionHook(__imp__MmQueryAddressProtect, MmQueryAddressProtect);
GuestFunctionStub(__imp__NetDll_WSAStartup);
GuestFunctionStub(__imp__NtAllocateVirtualMemory);
GuestFunctionHook(__imp__NtClose, NtClose);
GuestFunctionHook(__imp__NtCreateEvent, NtCreateEvent);
GuestFunctionHook(__imp__NtCreateFile, NtCreateFile);
GuestFunctionStub(__imp__NtCreateMutant); // TVG
GuestFunctionStub(__imp__NtDeviceIoControlFile); // TVG
GuestFunctionStub(__imp__NtDuplicateObject);
GuestFunctionStub(__imp__NtFlushBuffersFile);
GuestFunctionStub(__imp__NtFreeVirtualMemory);

GuestFunctionHook(__imp__NtOpenFile, NtOpenFile);
GuestFunctionHook(__imp__NtQueryDirectoryFile, NtQueryDirectoryFile);
GuestFunctionHook(__imp__NtQueryFullAttributesFile, NtQueryFullAttributesFile);
GuestFunctionHook(__imp__NtQueryInformationFile, NtQueryInformationFile);

GuestFunctionStub(__imp__NtQueryVirtualMemory);

GuestFunctionHook(__imp__NtQueryVolumeInformationFile, NtQueryVolumeInformationFile);
GuestFunctionHook(__imp__NtReadFile, NtReadFile);

GuestFunctionStub(__imp__NtReleaseMutant);
GuestFunctionHook(__imp__NtResumeThread, NtResumeThread);
GuestFunctionHook(__imp__NtSetEvent, NtSetEvent);

GuestFunctionHook(__imp__NtSetInformationFile, NtSetInformationFile);

GuestFunctionHook(__imp__NtSuspendThread, NtSuspendThread);
GuestFunctionStub(__imp__NtWaitForMultipleObjectsEx);
GuestFunctionHook(__imp__NtWaitForSingleObjectEx, NtWaitForSingleObjectEx);
GuestFunctionStub(__imp__NtWriteFile);
GuestFunctionStub(__imp__ObCreateSymbolicLink);
GuestFunctionStub(__imp__ObDeleteSymbolicLink);
GuestFunctionStub(__imp__ObDereferenceObject);
GuestFunctionHook(__imp__ObReferenceObjectByHandle, ObReferenceObjectByHandle);
GuestFunctionStub(__imp__RtlCompareMemoryUlong);
GuestFunctionHook(__imp__RtlEnterCriticalSection, RtlEnterCriticalSection);
GuestFunctionStub(__imp__RtlFillMemoryUlong);
GuestFunctionStub(__imp__RtlImageXexHeaderField);
GuestFunctionHook(__imp__RtlInitAnsiString, RtlInitAnsiString);
GuestFunctionHook(__imp__RtlInitializeCriticalSection, RtlInitializeCriticalSection);
GuestFunctionHook(__imp__RtlLeaveCriticalSection, RtlLeaveCriticalSection);
GuestFunctionHook(__imp__RtlMultiByteToUnicodeN, RtlMultiByteToUnicodeN);
GuestFunctionStub(__imp__RtlNtStatusToDosError);
GuestFunctionStub(__imp__RtlRaiseException);
GuestFunctionStub(__imp__RtlTimeToTimeFields);
GuestFunctionHook(__imp__RtlUnicodeToMultiByteN, RtlUnicodeToMultiByteN);
GuestFunctionStub(__imp__RtlUnwind);
GuestFunctionStub(__imp__StfsControlDevice);
GuestFunctionStub(__imp__StfsCreateDevice);
GuestFunctionStub(__imp__VdCallGraphicsNotificationRoutines);
GuestFunctionStub(__imp__VdEnableDisableClockGating);
GuestFunctionStub(__imp__VdEnableRingBufferRPtrWriteBack);
GuestFunctionStub(__imp__VdGetCurrentDisplayGamma);
GuestFunctionStub(__imp__VdGetCurrentDisplayInformation);
GuestFunctionStub(__imp__VdGetSystemCommandBuffer);
GuestFunctionStub(__imp__VdInitializeEngines);
GuestFunctionStub(__imp__VdInitializeRingBuffer);
GuestFunctionStub(__imp__VdInitializeScalerCommandBuffer);
GuestFunctionStub(__imp__VdIsHSIOTrainingSucceeded);
GuestFunctionHook(__imp__VdPersistDisplay, VdPersistDisplay);
GuestFunctionStub(__imp__VdQueryVideoFlags);
GuestFunctionHook(__imp__VdQueryVideoMode, VdQueryVideoMode);
GuestFunctionHook(__imp__VdRetrainEDRAM, StubReturn0);
GuestFunctionStub(__imp__VdRetrainEDRAMWorker);
GuestFunctionStub(__imp__VdSetDisplayMode);
GuestFunctionStub(__imp__VdSetGraphicsInterruptCallback);
GuestFunctionStub(__imp__VdSetSystemCommandBufferGpuIdentifierAddress);
GuestFunctionStub(__imp__VdShutdownEngines);
GuestFunctionStub(__imp__VdSwap);
GuestFunctionStub(__imp__XAudioGetVoiceCategoryVolume);
GuestFunctionHook(__imp__XAudioGetVoiceCategoryVolumeChangeMask, XAudioGetVoiceCategoryVolumeChangeMask);
GuestFunctionHook(__imp__XAudioRegisterRenderDriverClient, XAudioRegisterRenderDriverClient);
GuestFunctionHook(__imp__XAudioSubmitRenderDriverFrame, XAudioSubmitRenderDriverFrame);
GuestFunctionHook(__imp__XAudioUnregisterRenderDriverClient, XAudioUnregisterRenderDriverClient);
GuestFunctionHook(__imp__XGetAVPack, StubReturn0);
GuestFunctionHook(__imp__XGetGameRegion, XGetGameRegion);
GuestFunctionHook(__imp__XGetLanguage, XGetLanguage);
GuestFunctionHook(__imp__XGetVideoMode, VdQueryVideoMode);
GuestFunctionStub(__imp__XMACreateContext);
GuestFunctionStub(__imp__XMAReleaseContext);
GuestFunctionHook(__imp__XMsgInProcessCall, XMsgInProcessCall);
GuestFunctionStub(__imp__XMsgStartIORequest);
GuestFunctionStub(__imp__XMsgStartIORequestEx);
GuestFunctionHook(__imp__XNotifyGetNext, XNotifyGetNext);
GuestFunctionHook(__imp__XamContentClose, XamContentClose);
GuestFunctionHook(__imp__XamContentCreateEnumerator, XamContentCreateEnumerator);
GuestFunctionHook(__imp__XamContentCreateEx, XamContentCreateEx);
GuestFunctionStub(__imp__XamContentDelete);
GuestFunctionHook(__imp__XamContentGetCreator, XamContentGetCreator);
GuestFunctionHook(__imp__XamContentGetDeviceState, StubReturn0);
GuestFunctionHook(__imp__XamEnumerate, XamEnumerate);
GuestFunctionStub(__imp__XamGetExecutionId);
GuestFunctionHook(__imp__XamGetSystemVersion, StubReturn0);
GuestFunctionHook(__imp__XamInputGetCapabilities, XamInputGetCapabilities);
GuestFunctionHook(__imp__XamInputGetState, XamInputGetState);
GuestFunctionHook(__imp__XamInputSetState, XamInputSetState);
GuestFunctionStub(__imp__XamLoaderGetLaunchData); // TVG
GuestFunctionStub(__imp__XamLoaderLaunchTitle);
GuestFunctionStub(__imp__XamLoaderSetLaunchData); // TVG
GuestFunctionStub(__imp__XamLoaderTerminateTitle);
GuestFunctionHook(__imp__XamNotifyCreateListener, XamNotifyCreateListener);
GuestFunctionHook(__imp__XamShowDeviceSelectorUI, XamShowDeviceSelectorUI);
GuestFunctionStub(__imp__XamShowMessageBoxUIEx);
GuestFunctionStub(__imp__XamShowSigninUI);
GuestFunctionStub(__imp__XamTaskCloseHandle); // TVG
GuestFunctionStub(__imp__XamTaskSchedule); // TVG
GuestFunctionStub(__imp__XamTaskShouldExit); // TVG
GuestFunctionStub(__imp__XamUserCreateAchievementEnumerator); // TVG
GuestFunctionStub(__imp__XamUserGetName); // TVG
GuestFunctionHook(__imp__XamUserGetSigninState, StubReturn1);
GuestFunctionHook(__imp__XamUserReadProfileSettings, XamUserReadProfileSettings);
GuestFunctionStub(__imp__XeCryptSha);
GuestFunctionStub(__imp__XeKeysConsolePrivateKeySign); // TVG
GuestFunctionStub(__imp__XeKeysConsoleSignatureVerification); // TVG
GuestFunctionStub(__imp__XexGetModuleHandle);
GuestFunctionStub(__imp__XexGetProcedureAddress);
GuestFunctionHook(__imp__XexCheckExecutablePrivilege, StubReturn0);
GuestFunctionStub(__imp____C_specific_handler);
GuestFunctionStub(__imp___vsnprintf);
GuestFunctionStub(__imp__sprintf);
GuestFunctionStub(__imp__vswprintf);

GuestFunctionStub(__imp__vsprintf);
// GuestFunctionStub(__imp___vsnprintf);
// GuestFunctionStub(__imp__sprintf);
GuestFunctionStub(__imp___snprintf);
GuestFunctionStub(__imp___snwprintf);
// GuestFunctionStub(__imp__vswprintf);
GuestFunctionStub(__imp___vscwprintf);
GuestFunctionStub(__imp__swprintf);

