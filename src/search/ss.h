#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"

namespace elixir::search {
    struct SearchStack {
        move::Move move          = move::NO_MOVE;
        move::Move excluded_move = move::NO_MOVE;
        move::Move killers[2]    = {};
        int static_eval          = SCORE_NONE;
        int eval                 = SCORE_NONE;
        ContHistEntry *cont_hist = nullptr;
        int ply;
    };
}
