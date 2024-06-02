#pragma once

#include "move.h"

namespace elixir::search {
    using cont_hist_table = std::array<std::array<std::array<int, 64>, 6>, 2>;
    struct SearchStack {
        move::Move move = move::NO_MOVE;
        move::Move killers[2] = {};
        int eval;
        int ply;
        cont_hist_table cont_hist;
    };
}