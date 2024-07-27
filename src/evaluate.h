#pragma once

#include "board/board.h"
#include "evaluate.h"
#include "types.h"

namespace elixir {

    constexpr EvalScore S(const Score o_score, const Score e_score) {
        return static_cast<EvalScore>(((I32)e_score << 16) + (I32)o_score);
    }

    constexpr Score O(const EvalScore score) {
        return static_cast<Score>((I16)score);
    }

    constexpr Score E(const EvalScore score) {
        return static_cast<Score>((I16)((score + 0x8000) >> 16));
    }
    namespace eval {
        extern int TEMPO;
        extern int MP_PAWN;
        extern int MP_KNIGHT;
        extern int MP_BISHOP;
        extern int MP_ROOK;
        extern int MP_QUEEN;
        extern int MP_KING;
        extern int piece_values[7];

        extern int evaluate(Board &board);
    }
}