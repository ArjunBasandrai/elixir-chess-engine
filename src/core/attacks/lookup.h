#pragma once

#include "../defs.h"
#include "../types.h"
#include "magics.h"

namespace elixir::attacks {
    extern Bitboard pawn_attacks[2][64];
    extern Bitboard knight_attacks[64];
    extern Bitboard king_attacks[64];

    void init_pawn_attacks();
    void init_knight_attacks();
    void init_king_attacks();
}

namespace elixir::magic {
    // extern U64 bishop_magic_numbers[64];
    // extern U64 rook_magic_numbers[64];
    // void init_magic_numbers();

    void init_bishop_attacks();
    void init_rook_attacks();
}