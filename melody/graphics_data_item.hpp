#pragma once
#include "utils/xbox.h"

struct GraphicsDataItemVTable {
	xpointer<void> scalar_deleting_destructor;
	xpointer<void> SendData;
};

struct GraphicsDataItem {
	xpointer<GraphicsDataItemVTable> vt;
	be<unsigned int> unk;
};