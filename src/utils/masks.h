#pragma once

#include "../types.h"
#include "../defs.h"

namespace elixir {
    extern Bitboard connected_pawn_masks[2][64];
    void init_masks();
}