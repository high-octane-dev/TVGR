#include <bit>
#include "runtime/hook.hpp"
#include "runtime/logger.hpp"

void CarsSettings_Unk(void* _this) {
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(_this) + 0x78) = 0x01000000;
	*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(_this) + 0x74) = 0x01000000;
}

// TVG attempts to lock an object from an absolute address (WTF?!)
GuestFunctionStub(sub_820E0EF8);

// TVG calls operator new with a size of 0 and derefs (WTF? MN PC does this too). For some reason this doesnt work in UR.
GuestFunctionHook(sub_824AD260, CarsSettings_Unk);