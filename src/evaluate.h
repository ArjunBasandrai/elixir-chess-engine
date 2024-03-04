#pragma once

#include "board/board.h"

namespace elixir {
    constexpr inline EvalScore S(Score o_score, Score e_score) {
        return static_cast<EvalScore>((static_cast<U16>(o_score) << 16) | (static_cast<U16>(e_score) & 0xFFFF));
    }

    constexpr inline Score O(EvalScore score) {
        return static_cast<Score>(score >> 16);
    }

    constexpr inline Score E(EvalScore score) {
        return static_cast<Score>(score & 0xFFFF);
    }

    namespace eval {
        constexpr int piece_values[7] = {100, 300, 350, 500, 900, 20000, 0};
        constexpr EvalScore material_score[7] = {S(82, 94), S(337, 281), S(365, 297), S(477, 512), S(1025, 936), S(12000, 12000), S(0, 0)};
        Phase get_game_phase_score(Board &board, int &game_phase_score);
        int evaluate(Board& board);
        Score interpolate_eval(EvalScore score, Board &board);
    }
}