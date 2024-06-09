#pragma once

#include <vector>

#include "board/board.h"
#include "defs.h"
#include "move.h"
#include "types.h"
#include "utils/static_vector.h"

namespace elixir::movegen {
    constexpr int MAX_MOVES = 256;
    constexpr Bitboard sh_l(const Bitboard &b, int n) {
        if (n > 0)
            return b << n;
        else
            return b >> -n;
    }
    template <bool noisy> MoveList generate_moves(const Board &board);

    void generate_noisy_moves(const Board &board, MoveList &moves);
    void generate_quiet_moves(const Board &board, MoveList &moves);
}