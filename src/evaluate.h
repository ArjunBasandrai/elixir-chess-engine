#pragma once

#include "board/board.h"

namespace elixir::eval {
    constexpr int piece_values[7] = {100, 300, 350, 500, 900, 20000, 0};
    int evaluate(Board& board);
}