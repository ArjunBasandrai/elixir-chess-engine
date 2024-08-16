#pragma once

#include <chrono>
#include <array>

#include "defs.h"
#include "move.h"
#include "search.h"
#include "spsa.h"
#include "types.h"


namespace elixir {
    namespace time_management {
        extern double move_stability_scale[5];
        extern double node_fraction_base;
        extern double node_fraction_scale;
        class TimeManager {
          public:
            void optimum_time(search::SearchInfo &info, F64 time, F64 inc, int movestogo,
                              std::chrono::high_resolution_clock::time_point start_time_point);
            bool should_stop(search::SearchInfo &info);
            bool should_stop_early(search::SearchInfo &info, const int depth, const move::Move best_move, U64 nodes_searched);

            [[nodiscard]] U64 &nodes_spent(move::Move move) {
                return nodes_spent_[move.get_move() & 4095];
            }

          private:
            F64 time_elapsed(search::SearchInfo &info) {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock::now() - start_time)
                    .count();
            }
            move::Move prev_best_move = move::NO_MOVE;
            int best_move_stability = 0;
            std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
            F64 soft_limit;
            F64 hard_limit;

            std::array<U64, 4096> nodes_spent_;
        };
    }

    inline time_management::TimeManager time_manager;
}