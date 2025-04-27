#pragma once
#include "utils/xbox.h"

struct BaseObjectVTable {
	xpointer<void> scalar_deleting_destructor;
	xpointer<void> AddRef;
	xpointer<void> RemoveRef;
	xpointer<void> Release;
	xpointer<void> GetRefCount;
	xpointer<void> PreRelease;
};

struct BaseObject {
	xpointer<BaseObjectVTable> vt;
	be<std::int32_t> ref_count;
};