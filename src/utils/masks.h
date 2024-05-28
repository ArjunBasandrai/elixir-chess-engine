#pragma once

#include "../types.h"
#include "../defs.h"

namespace elixir::masks {
    constexpr Bitboard isolated_pawn_masks[8] = { 0x202020202020202, 0x505050505050505, 0xA0A0A0A0A0A0A0A, 0x1414141414141414, 0x2828282828282828, 0x5050505050505050, 0xA0A0A0A0A0A0A0A0, 0x4040404040404040 };
}