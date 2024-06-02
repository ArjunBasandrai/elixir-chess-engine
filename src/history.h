#pragma once

#include "types.h"
#include "defs.h"
#include "move.h"
#include "search_stack.h"

namespace elixir {

    class Board;
    class History {
    public:
        History() = default;
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
        int get_history(Square from, Square to) const;

        void update_cont_history(const Board& board, int color, int piece_type, int to, int depth, MoveList &bad_quiets, search::SearchStack *ss);
        int get_cont_history(int color, int piece_type, int to, const search::SearchStack *ss) const;

    private:
        int scale_bonus(int score, int bonus);
        const int HISTORY_GRAVITY = 8192;
        int history[64][64] = {0};
    };
}