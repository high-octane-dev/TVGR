#include <filesystem>
#include "runtime/kernel/memory.hpp"
#include "runtime/kernel/heap.hpp"
#include "runtime/kernel/xam.hpp"
#include "runtime/xaudio/xaudio.hpp"
#include "runtime/xenon/guest_thread.hpp"
#include "utils/xbox.h"
#include "utils/xex.h"
#include "utils/file.h"

Memory GLOBAL_MEMORY;
Heap GLOBAL_USER_HEAP;


void host_startup() {
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	GLOBAL_USER_HEAP.init();
	// hid::Init();
}

void guest_startup() {
    const auto game_content = XamMakeContent(XCONTENTTYPE_RESERVED, "Game");
    XamRegisterContent(game_content, "/Data");
    XamRegisterContent(XamMakeContent(XCONTENTTYPE_SAVEDATA, "Save"), "Save");

    // Mount game
    XamContentCreateEx(0, "game", &game_content, OPEN_EXISTING, nullptr, nullptr, 0, 0, nullptr);
    // OS mounts game data to D:
    XamContentCreateEx(0, "D", &game_content, OPEN_EXISTING, nullptr, nullptr, 0, 0, nullptr);

    XAudioInitializeSystem();
}


uint32_t LdrLoadModule(const std::filesystem::path& path)
{
    auto loadResult = LoadFile(path);
    if (loadResult.empty())
    {
        assert("Failed to load module" && false);
        return 0;
    }

    auto* header = reinterpret_cast<const Xex2Header*>(loadResult.data());
    auto* security = reinterpret_cast<const Xex2SecurityInfo*>(loadResult.data() + header->securityOffset);
    const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_FILE_FORMAT_INFO));
    auto entry = *reinterpret_cast<const uint32_t*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_ENTRY_POINT));
    ByteSwapInplace(entry);

    auto srcData = loadResult.data() + header->headerSize;
    auto destData = reinterpret_cast<uint8_t*>(GLOBAL_MEMORY.translate(security->loadAddress));

    if (fileFormatInfo->compressionType == XEX_COMPRESSION_NONE)
    {
        memcpy(destData, srcData, security->imageSize);
    }
    else if (fileFormatInfo->compressionType == XEX_COMPRESSION_BASIC)
    {
        auto* blocks = reinterpret_cast<const Xex2FileBasicCompressionBlock*>(fileFormatInfo + 1);
        const size_t numBlocks = (fileFormatInfo->infoSize / sizeof(Xex2FileBasicCompressionInfo)) - 1;

        for (size_t i = 0; i < numBlocks; i++)
        {
            memcpy(destData, srcData, blocks[i].dataSize);

            srcData += blocks[i].dataSize;
            destData += blocks[i].dataSize;

            memset(destData, 0, blocks[i].zeroSize);
            destData += blocks[i].zeroSize;
        }
    }
    else
    {
        assert(false && "Unknown compression type.");
    }

    auto res = reinterpret_cast<const Xex2ResourceInfo*>(getOptHeaderPtr(loadResult.data(), XEX_HEADER_RESOURCE_INFO));

    return entry;
}

int main() {
	timeBeginPeriod(1);
	host_startup();
	guest_startup();

	uint32_t entry = LdrLoadModule("Default_TVG_Decrypted.xex");

	GuestThread::Start({ entry, 0, 0 });
	return 0;
}