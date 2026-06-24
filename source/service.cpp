#include <3ds.h>
#include <cstring>

#include "config.h"
#include "hardware.h"
#include "kernel.h"
#include "misc.h"
#include "nnid.h"
#include "service.h"
#include "storage.h"
#include "system.h"
#include "utils.h"
#include "wifi.h"

namespace ACI {
    Result GetSecurityMode(acSecurityMode* mode) {
        Result ret = 0;
        u32* cmdbuf = getThreadCommandBuffer();

        cmdbuf[0] = IPC_MakeHeader(0x413, 0, 0); // 0x04130000

        if (R_FAILED(ret = svcSendSyncRequest(*acGetSessionHandle()))) {
            return ret;
        }

        *mode = static_cast<acSecurityMode>(cmdbuf[2]);
        return static_cast<Result>(cmdbuf[1]);
    }

    Result GetPassphrase(char* passphrase) {
        Result ret = 0;
        u32* cmdbuf = getThreadCommandBuffer();

        cmdbuf[0] = IPC_MakeHeader(0x415, 0, 0); // 0x04150000

        u32* staticbufs = getThreadStaticBuffers();
        staticbufs[0] = IPC_Desc_StaticBuffer(64, 0); // Password length is 64
        staticbufs[1] = reinterpret_cast<u32>(passphrase);

        if (R_FAILED(ret = svcSendSyncRequest(*acGetSessionHandle()))) {
            return ret;
        }

        return static_cast<Result>(cmdbuf[1]);
    }
} // namespace ACI

namespace MCUHWC {
    Result GetBatteryTemperature(u8* temp) {
        Result ret = 0;
        u32* cmdbuf = getThreadCommandBuffer();

        cmdbuf[0] = IPC_MakeHeader(0xE, 2, 0); // 0x000E0080

        if (R_FAILED(ret = svcSendSyncRequest(*mcuHwcGetSessionHandle()))) {
            return ret;
        }

        *temp = cmdbuf[2];
        return static_cast<Result>(cmdbuf[1]);
    }
} // namespace MCUHWC

namespace Service {
    void Init() {
        acInit();
        actInit(true);
        ACT_Initialize(0xB0002F0, 0, 0);
        amInit();
    }

    void Exit() {
        amExit();
        actExit();
        acExit();
    }

    KernelInfo GetKernelInfo() {
        KernelInfo info = {nullptr};
        info.kernelVersion = Kernel::GetVersion(VersionInfo::Kernel);
        info.firmVersion = Kernel::GetVersion(VersionInfo::Firm);
        info.systemVersion = Kernel::GetVersion(VersionInfo::System);
        info.initialVersion = Kernel::GetInitialVersion();
        info.sdmcCid = Kernel::GetSdmcCid();
        info.nandCid = Kernel::GetNandCid();
        info.deviceId = Kernel::GetDeviceId();
        return info;
    }

    SystemInfo GetSystemInfo() {
        SystemInfo info = {nullptr};
        info.model = System::GetModel();
        info.hardware = System::GetRunningHW();
        info.region = System::GetRegion();
        info.language = System::GetLanguage();
        info.localFriendCodeSeed = System::GetLocalFriendCodeSeed();
        info.nandLocalFriendCodeSeed = System::GetNandLocalFriendCodeSeed();
        info.macAddress = System::GetMacAddress();
        info.serialNumber = System::GetSerialNumber();
        info.checkDigit = (info.serialNumber != nullptr) ? System::GetCheckDigit(info.serialNumber) : 0;
        info.soapId = System::GetSoapId();
        return info;
    }

    NNIDInfo GetNNIDInfo() {
        NNIDInfo info = {0};
        info.persistentID = NNID::GetPersistentId();
        info.transferableIdBase = NNID::GetTransferableIdBase();
        info.accountId = NNID::GetAccountId();
        info.countryName = NNID::GetCountryName();
        info.principalID = NNID::GetPrincipalId();
        info.status = NNID::IsServerAccountDeleted();
        return info;
    }

    ConfigInfo GetConfigInfo() {
        ConfigInfo info = {nullptr};
        info.username = Config::GetUsername();
        info.birthday = Config::GetBirthday();
        info.eulaVersion = Config::GetEulaVersion();
        info.parentalPin = Config::GetParentalPin();
        info.parentalEmail = Config::GetParentalEmail();
        info.parentalSecretAnswer = Config::GetParentalSecretAnswer();
        return info;
    }

    HardwareInfo GetHardwareInfo() {
        HardwareInfo info = {nullptr};

        GspLcdScreenType top = GspLcdScreenType::Unknown;
        GspLcdScreenType bottom = GspLcdScreenType::Unknown;
        Hardware::GetScreenType(top, bottom);

        if (top == GspLcdScreenType::Unknown) {
            info.screenUpper = "unknown";
        }
        else {
            info.screenUpper = (top == GspLcdScreenType::TN) ? "TN" : "IPS";
        }

        if (bottom == GspLcdScreenType::Unknown) {
            info.screenLower = "unknown";
        }
        else {
            info.screenLower = (bottom == GspLcdScreenType::TN) ? "TN" : "IPS";
        }

        info.soundOutputMode = Hardware::GetSoundOutputMode();
        info.autoBrightnessStatus = Hardware::GetAutoBrightnessStatus();
        return info;
    }

    MiscInfo GetMiscInfo() {
        MiscInfo info = {0};
        info.sdTitleCount = Misc::GetTitleCount(MEDIATYPE_SD);
        info.nandTitleCount = Misc::GetTitleCount(MEDIATYPE_NAND);
        info.ticketCount = Misc::GetTicketCount();
        info.manufacturingDate = Misc::GetManufacturingDate();
        return info;
    }

    WifiInfo GetWifiInfo() {
        WifiInfo info = {false};

        for (u32 i = 0; i < 3; i++) {
            if (R_SUCCEEDED(ACI_LoadNetworkSetting(i))) {
                info.slot[i] = true;
                std::strncpy(info.ssid[i], Wifi::GetSSID(), 32);
                std::strncpy(info.passphrase[i], Wifi::GetPassphrase(), 64);
                std::strncpy(info.securityMode[i], Wifi::GetSecurityMode(), 12);
            }
        }

        return info;
    }

    StorageInfo GetStorageInfo() {
        StorageInfo info = {0};

        for (int i = 0; i < 4; i++) {
            u64 free = 0;
            u64 used = 0;
            u64 total = 0;
            Storage::GetStorageForMedia(static_cast<FS_SystemMediaType>(i), free, used, total);
            info.usedSize[i] = used;
            info.totalSize[i] = total;
            Utils::GetSizeString(info.freeSizeString[i], free);
            Utils::GetSizeString(info.usedSizeString[i], used);
            Utils::GetSizeString(info.totalSizeString[i], total);
        }

        Utils::GetSizeString(info.clusterSizeString, Storage::GetClusterSize());

        return info;
    }

    SystemStateInfo GetSystemStateInfo() {
        mcuHwcInit();
        SystemStateInfo info = {0};

        if (R_FAILED(MCUHWC_ReadRegister(0x7F, std::addressof(info), sizeof(SystemStateInfo)))) {
            mcuHwcExit();
            return info;
        }

        MCUHWC_GetFwVerHigh(std::addressof(info.mcuFwVerHigh));
        MCUHWC_GetFwVerLow(std::addressof(info.mcuFwVerLow));

        mcuHwcExit();
        return info;
    }
} // namespace Service
