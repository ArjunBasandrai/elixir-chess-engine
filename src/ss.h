#pragma once

#include "defs.h"
#include "types.h"
#include "move.h"

namespace elixir::search {
    struct SearchStack {
        move::Move move       = move::NO_MOVE;
        move::Move killers[2] = {};
        int eval = -INF;
        int ply;
        ContHistEntry* cont_hist = nullptr;
    };
}
