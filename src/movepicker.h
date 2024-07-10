#pragma once

#include "move.h"
#include "movegen.h"
#include "search.h"
#include "ss.h"
#include "utils/static_vector.h"

namespace elixir {
    extern int MP_SEE;
    class MovePicker {
      public:
        MovePicker()  = default;
        ~MovePicker() = default;
        void init_mp(const Board &board, move::Move tt_move, search::SearchStack *ss, bool for_qs);
        move::Move next_move();
        move::Move first_move() {
            if (moves.size() == 0)
                return move::NO_MOVE;
            return moves[0];
        }

      private:
        MoveList moves;
        StaticVector<int, 256> scores;
        void score_moves(const Board &board, const move::Move &tt_move,
                         const search::SearchStack *ss);
    };
}