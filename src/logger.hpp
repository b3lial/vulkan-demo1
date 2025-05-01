#pragma once

#ifdef DEBUG
#include <iostream>
#include <string>

namespace Log {
    inline void debug(const std::string& msg) {
        std::cout << "[DEBUG] " << msg << std::endl;
    }
    // optional: weitere Level wie info(), warn(), error()
}

#define LOG_DEBUG(msg) Log::debug(msg)

#else
// Im Release-Build: Logging komplett entfernen
#define LOG_DEBUG(msg) do {} while(0)
#endif