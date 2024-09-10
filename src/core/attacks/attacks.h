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
    inline Bitboard get_pawn_attacks(Color c, Square sq) noexcept {
        return pawn_attacks[static_cast<int>(c)][static_cast<int>(sq)];
    }
    inline Bitboard get_knight_attacks(Square sq) noexcept {
        return knight_attacks[static_cast<int>(sq)];
    }
    inline Bitboard get_king_attacks(Square sq) noexcept {
        return king_attacks[static_cast<int>(sq)];
    }
    [[nodiscard]] inline Bitboard get_bishop_attacks(Square sq, U64 occupancy) noexcept {
        int square = static_cast<int>(sq);
        occupancy &= magic::bishop_masks[square];
        occupancy *= magic::bishop_magic_numbers[square];
        occupancy >>= 64 - magic::bishop_relevant_bits[square];
        return magic::bishop_attacks[square][occupancy];
    }

    [[nodiscard]] inline Bitboard get_rook_attacks(Square sq, U64 occupancy) noexcept {
        int square = static_cast<int>(sq);
        occupancy &= magic::rook_masks[square];
        occupancy *= magic::rook_magic_numbers[square];
        occupancy >>= 64 - magic::rook_relevant_bits[square];
        return magic::rook_attacks[square][occupancy];
    }

    [[nodiscard]] inline Bitboard get_queen_attacks(Square sq, U64 occupancy) noexcept {
        return get_bishop_attacks(sq, occupancy) | get_rook_attacks(sq, occupancy);
    }
}