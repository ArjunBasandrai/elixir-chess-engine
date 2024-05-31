#pragma once

#include "board/board.h"
#include "types.h"
#include "evaluate.h"

namespace elixir {
        
    constexpr EvalScore S(Score o_score, Score e_score) {
        return static_cast<EvalScore>(((I32) e_score << 16) + (I32) o_score);
    }

    constexpr Score O(EvalScore score) {
        return static_cast<Score>((I16) score);
    }

    constexpr Score E(EvalScore score) {
        return static_cast<Score>((I16) ((score + 0x8000) >> 16));
    }
    namespace eval {
        constexpr int piece_values[7] = {100, 300, 350, 500, 900, 20000, 0};
        struct EvalInfo {
            EvalInfo(EvalScore score) : score(score) {}
            EvalScore score = 0;
            void add_score(EvalScore s) { score += s; }
            Score opening_score() { return O(score); }
            Score endgame_score() { return E(score); }
        };
    }

    namespace eval {
        int evaluate(Board& board);
    }
}