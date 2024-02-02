#pragma once

#include "magics.h"
#include "../types.h"
#include "../defs.h"

namespace elixir::attacks {
    constexpr Bitboard not_a_file = 18374403900871474942ULL;
    constexpr Bitboard not_h_file = 9187201950435737471ULL;
    constexpr Bitboard not_gh_file = 4557430888798830399ULL;
    constexpr Bitboard not_ab_file = 18229723555195321596ULL;

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