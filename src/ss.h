#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"


namespace elixir::search {
    struct SearchStack {
        move::Move move          = move::NO_MOVE;
        move::Move excluded_move = move::NO_MOVE;
        move::Move killers[2]    = {};
        I16 eval                 = -INF;
        ContHistEntry *cont_hist = nullptr;
        U8 ply;
    };
}
