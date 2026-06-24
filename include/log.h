#pragma once

#include <3ds.h>

namespace Log {
    Result Open();
    Result Close();
    void Error(const char* data, ...);
} // namespace Log
