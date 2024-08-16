#include <chrono>

#include "time_manager.h"

#include "defs.h"
#include "search.h"
#include "spsa.h"
#include "types.h"


namespace elixir::time_management {
    double move_stability_scale[5] = {
        2.43, 1.35, 1.09, 0.88, 0.68
    };

    double node_fraction_base = 1.52;
    double node_fraction_scale = 1.74;

    void TimeManager::optimum_time(search::SearchInfo &info, F64 time, F64 inc, int movestogo,
                                   std::chrono::high_resolution_clock::time_point start_time_point) {
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

        soft_limit = std::min(0.76 * base_time, max_bound);
        hard_limit = std::min(2.50 * base_time, max_bound);

        info = search::SearchInfo(MAX_DEPTH, true);

        start_time = start_time_point;
        nodes_spent_.fill(0);
    }

    bool TimeManager::should_stop(search::SearchInfo &info) {
        if (info.timed && ! (info.nodes & 1023) && time_elapsed(info) > hard_limit) {
            info.stopped = true;
            return true;
        }
        return false;
    }

    bool TimeManager::should_stop_early(search::SearchInfo &info, const int depth,
                                        const move::Move best_move, U64 nodes_searched) {
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

        const auto percent_nodes = nodes_spent(best_move) / std::max<double>(1, nodes_searched);
        const double percent_scale_factor = (node_fraction_base - percent_nodes) * node_fraction_scale;
        const double stability_scale = move_stability_scale[best_move_stability];
        const F64 optimal_time = std::min<F64>(soft_limit * percent_scale_factor * stability_scale, hard_limit);

        return time_elapsed(info) >= optimal_time;
    }
}