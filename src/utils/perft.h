#pragma once

#include "../board/board.h"

namespace elixir {
    void perft_driver(Board &board, int depth, long long &nodes);
    void perft_test(Board &board, int depth, long long &nodes);
}