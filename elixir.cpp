#include <iostream>
#include <cstdlib>

#include "src/types.h"
#include "src/defs.h"
#include "src/move.h"
#include "src/utils/bits.h"
#include "src/utils/test_fens.h"
#include "src/board/board.h"
#include "src/hashing/hash.h"
#include "src/attacks/lookup.h"
#include "src/attacks/magics.h"
#include "src/attacks/attacks.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
}

int main() {
    init();

    Bitboard occ = 0ULL;
    bits::set_bit(occ, Square::C5);
    bits::set_bit(occ, Square::F2);
    bits::set_bit(occ, Square::G7);
    bits::set_bit(occ, Square::B2);
    bits::set_bit(occ, Square::G5);
    bits::set_bit(occ, Square::E2);
    bits::set_bit(occ, Square::E7);
    bits::set_bit(occ, Square::D6);
    bits::set_bit(occ, Square::C4);
    print_bitboard(occ);
    print_bitboard(attacks::get_queen_attacks(Square::D4, occ));
    print_bitboard(attacks::get_bishop_attacks(Square::D4, occ));
    print_bitboard(attacks::get_rook_attacks(Square::E5, occ));
    print_bitboard(attacks::get_pawn_attacks(Color::WHITE, Square::B7));
    print_bitboard(attacks::get_knight_attacks(Square::D7));
    print_bitboard(attacks::get_king_attacks(Square::H4));

    return 0;
}