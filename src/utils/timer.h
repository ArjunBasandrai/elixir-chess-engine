#pragma once

#include "../types.h"

// from Stormphrax
namespace elixir::timer {
    class Timer {
    public:
        Timer();
        ~Timer() = default;

        [[nodiscard]] auto time() const -> F64;

		[[nodiscard]] static auto rough_time_ms() -> I64;
    private:
    #if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
        U64 init_time;
        F64 m_frequency;
    #else
        F64 init_time;
    #endif
    };

    inline const Timer m_timer{};
}