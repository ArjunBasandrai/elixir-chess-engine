#pragma once

#include "defs.h"
#include "move.h"

namespace elixir {
    class CounterMoveHistory {
        public:
            CounterMoveHistory() {
                clear();
            }
            ~CounterMoveHistory() = default;

            void clear();

            void update_countermove(Color side, Square from, Square to, move::Move countermove);
            move::Move get_countermove(Color side, Square from, Square to) const;
        
        private:
            move::Move counter_moves[2][64][64] = {move::NO_MOVE};
    };
}