#pragma once

#include "board/board.h"

namespace elixir {
    constexpr inline EvalScore S(int o_score, int e_score) {
        return static_cast<EvalScore>((static_cast<U16>(o_score) << 16) | (static_cast<U16>(e_score) & 0xFFFF));
    }

    constexpr inline int O(EvalScore score) {
        return static_cast<int>(score >> 16);
    }

    constexpr inline int E(EvalScore score) {
        return static_cast<int>(score & 0xFFFF);
    }

    namespace eval {
        constexpr int piece_values[7] = {100, 300, 350, 500, 900, 20000, 0};
        int evaluate(Board& board);
    }
}