#pragma once

#include "defs.h"
#include "move.h"

namespace elixir {
    using CounterMoveArray = std::vector<std::array<std::array<move::Move, 64>, 64>>;

    class CounterMoveHistory {
      public:
        CounterMoveHistory() {
            counter_moves.resize(2);
            clear();
        }
        ~CounterMoveHistory() = default;

        void clear();

        void update_countermove(Color side, Square from, Square to, move::Move countermove);
        move::Move get_countermove(Color side, Square from, Square to) const;

      private:
        CounterMoveArray counter_moves;
    };
}