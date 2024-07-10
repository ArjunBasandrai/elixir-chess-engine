#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"

namespace elixir {
    extern int HISTORY_GRAVITY;
    class History {
      public:
        History()  = default;
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
        int get_history(Square from, Square to) const;

        void update_countermove(Color side, Square from, Square to, move::Move countermove);
        move::Move get_countermove(Color side, Square from, Square to) const;

      private:
        int history_bonus(int depth) {
            return std::min(300 * depth - 150, 4000);
        }
        int scale_bonus(int score, int bonus);
        int history[64][64]                 = {0};
        move::Move counter_moves[2][64][64] = {move::NO_MOVE};
    };
}