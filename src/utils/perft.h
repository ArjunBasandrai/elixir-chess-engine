#pragma once

#include "../board/board.h"

namespace elixir {
    void perft_driver(Board& board, int depth, long& nodes);
}