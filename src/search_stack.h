#pragma once

#include <array>

#include "move.h"

namespace elixir {
    using cont_hist_entry = std::array<std::array<std::array<int, 64>, 6>, 2>;
    using cont_hist_table = std::array<std::array<std::array<cont_hist_entry, 64>, 6>, 2>;
    namespace search {
        struct SearchStack {
            move::Move move = move::NO_MOVE;
            move::Move killers[2] = {};
            int eval;
            int ply;
            cont_hist_entry *cont_hist = nullptr;
        };
    }
}