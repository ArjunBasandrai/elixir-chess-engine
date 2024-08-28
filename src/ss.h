#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"


namespace elixir::search {
    struct SearchStack {
        move::Move move          = move::NO_MOVE;
        move::Move excluded_move = move::NO_MOVE;
        move::Move killers[2]    = {};
        int eval                 = -INF;
        ContHistEntry *cont_hist = nullptr;
        bool in_check            = false;
        int ply;
    };
}
