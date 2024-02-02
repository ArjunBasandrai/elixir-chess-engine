#pragma once

#include "lookup.h"

namespace elixir::attacks {
    inline void init_attacks() {
        init_pawn_attacks();
        init_knight_attacks();
        init_king_attacks();
        magic::init_bishop_attacks();
        magic::init_rook_attacks();
    }
    constexpr Bitboard get_pawn_attacks(Color c, Square sq) { return pawn_attacks[static_cast<int>(c)][static_cast<int>(sq)]; }
    constexpr Bitboard get_knight_attacks(Square sq) { return knight_attacks[static_cast<int>(sq)]; }
    constexpr Bitboard get_king_attacks(Square sq) { return king_attacks[static_cast<int>(sq)]; }
    extern Bitboard get_rook_attacks(Square sq, U64 occupancy);
    extern Bitboard get_bishop_attacks(Square sq, U64 occupancy);
}