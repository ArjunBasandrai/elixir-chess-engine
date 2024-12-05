#include <chrono>

#include "time_management_terms.h"
#include "time_manager.h"

#include "defs.h"
#include "search.h"
#include "spsa/spsa.h"
#include "types.h"

namespace elixir::time_management {
    double move_stability_scale[5] = {2.43, 1.35, 1.09, 0.88, 0.68};

    void TimeManager::optimum_time(search::SearchInfo &info, F64 time, F64 inc, int movestogo,
                                   std::chrono::high_resolution_clock::time_point start_time_point,
                                   bool is_movetime) {
        if (time < 0)
            time = 1000;

        time -= DEFAULT_MOVE_OVERHEAD;

        if (is_movetime) {
            soft_limit = time;
            hard_limit = time;
        } else {
            double base_time;

            if (movestogo != -1) {
                base_time = time / movestogo;
            } else {
                base_time = time * (BASE_SCALE / 1000.0) + inc * (INC_SCALE / 100.0);
            }
            const auto max_bound = (MAX_BOUND_SCALE / 100.0) * time;

            soft_limit = std::min((SOFT_BOUND_SCALE / 100.0) * base_time, max_bound);
            hard_limit = std::min((HARD_BOUND_SCALE / 100.0) * base_time, max_bound);
        }

        info = search::SearchInfo(MAX_DEPTH, true);

        start_time = start_time_point;
    }

    bool TimeManager::should_stop(search::SearchInfo &info) {
        if (info.timed && ! (info.nodes & 1023) && time_elapsed(info) > hard_limit) {
            info.stopped = true;
            return true;
        }
        return false;
    }

    bool TimeManager::should_stop_early(search::SearchInfo &info, const int depth,
                                        const move::Move best_move) {
        if (! info.timed)
            return false;

        if (depth < 7) {
            if (time_elapsed(info) > soft_limit) {
                info.stopped = true;
                return true;
            }
        }

        if (prev_best_move != best_move) {
            prev_best_move      = best_move;
            best_move_stability = 0;
        } else if (best_move_stability < 4) {
            best_move_stability++;
        }

        const double stability_scale = move_stability_scale[best_move_stability];
        const F64 optimal_time       = std::min<F64>(soft_limit * stability_scale, hard_limit);

        return time_elapsed(info) >= optimal_time;
    }
}