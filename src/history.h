#pragma once

#include "types.h"
#include "defs.h"
#include "move.h"

namespace elixir {
    using namespace move;
    class History {
    public:
        History() = default;
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
        int get_history(Square from, Square to) const;

        void update_capt_hist(Move move, PieceType captured, int depth);
        int get_capt_hist(Move move, PieceType captured) const;

    private:
        int scale_bonus(int score, int bonus);
        const int HISTORY_GRAVITY = 8192;
        int history[64][64] = {0};
        int capt_hist[12][64][6] = {0};
    };
}