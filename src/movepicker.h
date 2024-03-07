#pragma once

#include "utils/static_vector.h"
#include "move.h"
#include "movegen.h"
#include "search.h"

namespace elixir {
    class MovePicker {
    public:
        MovePicker() = default;
        ~MovePicker() = default;
        void init_mp(Board& baord, move::Move tt_move, search::SearchStack *ss, bool for_qs);
        MoveList get_moves() { return moves; }
        move::Move next_move();
    private:
        MoveList moves;
        StaticVector<int, 256> scores;
        void score_moves(Board &board, move::Move tt_move, search::SearchStack *ss);
        void sort_moves(Board &board, move::Move tt_move, search::SearchStack *ss);
        int index;
    };
}