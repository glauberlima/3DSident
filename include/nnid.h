#pragma once

namespace NNID {
    u32 GetPersistentId();
    u64 GetTransferableIdBase();
    const char* GetAccountId();
    const char* GetCountryName();
    u32 GetPrincipalId();
    const char* IsServerAccountDeleted();
} // namespace NNID
