#pragma once

#include <chrono>

#include "defs.h"
#include "move.h"
#include "search.h"
#include "spsa.h"
#include "types.h"


namespace elixir {
    namespace time_management {
        class TimeManager {
          public:
            void optimum_time(search::SearchInfo &info, F64 time, F64 inc, int movestogo,
                              std::chrono::high_resolution_clock::time_point start_time);
            bool should_stop(search::SearchInfo &info);
            bool should_stop_early(search::SearchInfo &info);

          private:
            auto time_elapsed(search::SearchInfo &info) {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock::now() - info.start_time)
                    .count();
            }
            move::Move prev_best_move = move::NO_MOVE;
        };
    }

    inline time_management::TimeManager time_manager;
}