#pragma once

#include "board/board.h"

namespace elixir::eval {
    constexpr int piece_values[6] = {100, 300, 350, 500, 900, 20000};
    int evaluate(Board& board);
}