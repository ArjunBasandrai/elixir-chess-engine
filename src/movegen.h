#pragma once

#include <vector>

#include "types.h"
#include "defs.h"
#include "utils/static_vector.h"
#include "board/board.h"
#include "move.h"

namespace elixir::movegen {
    constexpr int MAX_MOVES = 256;
    constexpr Bitboard sh_l(const Bitboard& b, int n) { 
        if (n > 0) return b << n;
        else return b >> -n;
    }

    MoveList generate_moves(const Board& board);

    void generate_noisy_moves(const Board& board, MoveList& moves);
    void generate_quiet_moves(const Board& board, MoveList& moves);
}