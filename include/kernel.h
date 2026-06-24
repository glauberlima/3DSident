#pragma once

#include <cstdint>

enum class VersionInfo : std::uint8_t { Kernel = 0, Firm, System };

namespace Kernel {
    const char* GetInitialVersion();
    const char* GetVersion(VersionInfo info);
    const char* GetSdmcCid();
    const char* GetNandCid();
    u32 GetDeviceId();
} // namespace Kernel
