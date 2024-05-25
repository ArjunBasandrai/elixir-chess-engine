#include "timer.h"

// from Stormphrax
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#include <Windows.h>
namespace elixir::timer {
	Timer::Timer() {
		U64 freq{};
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&freq));

		m_frequency = static_cast<F64>(freq);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&init_time));
	}

	auto Timer::time() const -> F64 {
		U64 time{};
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&time));

		return static_cast<F64>(time - init_time) * 1000.0 / m_frequency;
	}

	auto Timer::rough_time_ms() -> I64 {
		return static_cast<I64>(GetTickCount64());
	}
}
#else 
#include <time.h>
namespace elixir::timer
{
	Timer::Timer()
	{
		struct timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);

		init_time = static_cast<F64>(time.tv_sec) + static_cast<F64>(time.tv_nsec) / 1000.0;
	}

	auto Timer::time() const -> F64
	{
		struct timespec time{};
		clock_gettime(CLOCK_MONOTONIC, &time);

		return (static_cast<F64>(time.tv_sec) + static_cast<F64>(time.tv_nsec) / 1000.0) - init_time;
	}
}
#endif
