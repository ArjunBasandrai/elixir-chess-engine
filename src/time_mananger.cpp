#include <chrono>

#include "time_manager.h"

#include "defs.h"
#include "search.h"
#include "spsa.h"
#include "types.h"


namespace elixir::time_management {
    void TimeManager::optimum_time(search::SearchInfo &info, F64 time, F64 inc, int movestogo,
                                   std::chrono::high_resolution_clock::time_point start_time) {
        if (time < 0)
            time = 1000;

        time -= DEFAULT_MOVE_OVERHEAD;

        double base_time;

        if (movestogo != -1) {
            base_time = time / movestogo;
        } else {
            base_time = time * 0.054 + inc * 0.6;
        }
        const auto max_bound = 0.76 * time;

        const auto soft_bound = std::min(0.76 * base_time, max_bound);
        const auto hard_bound = std::min(2.50 * base_time, max_bound);

        info = search::SearchInfo(MAX_DEPTH, start_time, soft_bound, hard_bound);
    }

    bool TimeManager::should_stop(search::SearchInfo &info) {
        if (info.timed && ! (info.nodes & 1023) && time_elapsed(info) > info.hard_limit) {
            info.stopped = true;
            return true;
        }
        return false;
    }

    bool TimeManager::should_stop_early(search::SearchInfo &info) {
        if (info.timed && time_elapsed(info) > info.soft_limit) {
            info.stopped = true;
            return true;
        }
        return false;
    }
}