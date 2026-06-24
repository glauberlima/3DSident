#pragma once

namespace System {
    const char* GetModel();
    const char* GetRegion();
    const char* GetFirmRegion();
    bool IsCoppacsSupported();
    const char* GetLanguage();
    const char* GetMacAddress();
    const char* GetRunningHW();
    u64 GetLocalFriendCodeSeed();
    const char* GetNandLocalFriendCodeSeed();
    u8* GetSerialNumber();
    int GetCheckDigit(const u8* serialNumber);
    u64 GetSoapId();
} // namespace System
