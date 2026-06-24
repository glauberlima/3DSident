#include "hardware.h"
#include "log.h"
#include "utils.h"

#define REG_LCD_TOP_SCREEN static_cast<u32>(0x202200)
#define REG_LCD_BOTTOM_SCREEN static_cast<u32>(0x202A00)

namespace Hardware {
    struct AutoBrightnessBlock {
        u32 unk1;
        bool autoBrightnessEnabled;
        u8 unk2[3];
    };

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    Result GetScreenType(GspLcdScreenType& top, GspLcdScreenType& bottom) {
        Result ret = 0;
        u8 vendors = 0;

        if (!Utils::IsNew3DS()) {
            top = GspLcdScreenType::TN;
            bottom = GspLcdScreenType::TN;
            return 0;
        }

        if (R_FAILED(ret = gspLcdInit())) {
            Log::Error("%s(gspLcdInit) failed: 0x%x\n", __func__, ret);
            return ret;
        }

        if (R_FAILED(ret = GSPLCD_GetVendors(&vendors))) {
            Log::Error("%s(GSPLCD_GetVendors) failed: 0x%x\n", __func__, ret);
            return ret;
        }

        switch ((vendors >> 4) & 0xF) {
        case 0x01: // JDI => IPS
            top = GspLcdScreenType::IPS;
            break;
        case 0x0C: // SHARP => TN
            top = GspLcdScreenType::TN;
            break;
        default:
            top = GspLcdScreenType::Unknown;
            break;
        }

        switch (vendors & 0xF) {
        case 0x01: // JDI => IPS
            bottom = GspLcdScreenType::IPS;
            break;
        case 0x0C: // SHARP => TN
            bottom = GspLcdScreenType::TN;
            break;
        default:
            bottom = GspLcdScreenType::Unknown;
            break;
        }

        gspLcdExit();
        return 0;
    }

    bool GetAudioJackStatus() {
        Result ret = 0;
        bool status = false;

        if (R_FAILED(ret = DSP_GetHeadphoneStatus(std::addressof(status)))) {
            return false;
        }

        return status;
    }

    bool GetCardSlotStatus() {
        Result ret = 0;
        bool status = false;

        if (R_FAILED(ret = FSUSER_CardSlotIsInserted(std::addressof(status)))) {
            return false;
        }

        return status;
    }

    FS_CardType GetCardType() {
        Result ret = 0;
        FS_CardType type = CARD_CTR;

        if (R_FAILED(ret = FSUSER_GetCardType(std::addressof(type)))) {
            return CARD_CTR;
        }

        return type;
    }

    bool IsSdInserted() {
        Result ret = 0;
        bool detected = false;

        if (R_FAILED(ret = FSUSER_IsSdmcDetected(std::addressof(detected)))) {
            return false;
        }

        return detected;
    }

    const char* GetSoundOutputMode() {
        Result ret = 0;
        u8 data = 0;
        const char* const mode[] = {"Mono", "Stereo", "Surround"};

        if (R_FAILED(ret = CFGU_GetConfigInfoBlk2(sizeof(data), 0x00070001, std::addressof(data)))) {
            Log::Error("%s failed: 0x%x\n", __func__, ret);
            return "unknown";
        }

        if (data >= 3) {
            return "unknown";
        }
        return mode[data];
    }

    u32 GetBrightness(u32 screen) {
        Result ret = 0;
        u32 brightness = 0;
        u32 addr = (screen == GSPLCD_SCREEN_TOP ? REG_LCD_TOP_SCREEN : REG_LCD_BOTTOM_SCREEN) + 0x40;

        if (R_FAILED(ret = GSPGPU_ReadHWRegs(addr, std::addressof(brightness), 4))) {
            return 0;
        }

        return brightness;
    }

    const char* GetAutoBrightnessStatus() {
        AutoBrightnessBlock autoBrightnessBlock = {};

        if (R_FAILED(
                CFG_GetConfigInfoBlk8(sizeof(AutoBrightnessBlock), 0x00050009, std::addressof(autoBrightnessBlock)))) {
            return "unknown";
        }

        return autoBrightnessBlock.autoBrightnessEnabled ? "enabled" : "disabled";
    }
} // namespace Hardware
