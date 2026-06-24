#pragma once

#include <3ds.h>
#include <cstdint>

enum class GspLcdScreenType : std::uint8_t { TN, IPS, Unknown };

namespace Hardware {
    Result GetScreenType(GspLcdScreenType& top, GspLcdScreenType& bottom);
    bool GetAudioJackStatus();
    bool GetCardSlotStatus();
    FS_CardType GetCardType();
    bool IsSdInserted();
    const char* GetSoundOutputMode();
    u32 GetBrightness(u32 screen);
    const char* GetAutoBrightnessStatus();
} // namespace Hardware
