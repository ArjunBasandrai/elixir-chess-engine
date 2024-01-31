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

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_pawn_attacks();
}

int main() {
    init();
    
    print_bitboard(attacks::pawn_attacks[static_cast<int>(Color::BLACK)][static_cast<int>(Square::A4)]);
    print_bitboard(attacks::pawn_attacks[static_cast<int>(Color::BLACK)][static_cast<int>(Square::H4)]);
    print_bitboard(attacks::pawn_attacks[static_cast<int>(Color::BLACK)][static_cast<int>(Square::E4)]);
    
    return 0;
}