#pragma once

#include <vector>

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "move.h"

namespace elixir::movegen {
    constexpr int MAX_MOVES = 256;
    std::vector<move::Move> generate_moves(Board board);
}