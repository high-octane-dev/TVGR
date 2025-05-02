#include "utils/xbox.h"


struct VideoCardVTable {
	xpointer<void> scalar_deleting_destructor;
	xpointer<void> FreeCursor; // stubbed in console ports
	xpointer<void> ResetStateManager;
	xpointer<void> DisplayToScreen;
	xpointer<void> Unk0; // do nothing
	xpointer<void> Unk1; // do nothing
	xpointer<void> Unk2; // do nothing
	xpointer<void> Unk3; // do nothing
	xpointer<void> Unk4; // do nothing
};

struct X360VideoCard {
	xpointer<VideoCardVTable> vt;
	be<int> initialized;
	be<int> unused;
	be<int> should_inc_frame_count;
	be<int> unk_refresh_rate_param;
	be<int> unk_refresh_rate_param_2;
	be<int> unused2;
	be<int> unused3;
	// ContainerList<be<std::uint32_t>> unused4;
	xpointer<void> unused4;
	// XD3DCAPS9 capabilities;
	int current_frame;
	std::uint8_t flags;
	bool enable_fullscreen_effects;
};