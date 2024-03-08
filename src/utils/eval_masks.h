#pragma once

#include "../defs.h"
#include "../types.h"

namespace elixir {
    extern Bitboard isolated_pawn_mask[8];

    void init_masks();
}