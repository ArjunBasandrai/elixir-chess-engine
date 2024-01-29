#include <iostream>
#include <cstdlib>

#include "src/types.h"
#include "src/defs.h"
#include "src/move.h"
#include "src/utils/bits.h"
#include "src/utils/test_fens.h"
#include "src/board/board.h"
#include "src/hashing/hash.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
}

int main() {
    init();
    
    move::Move m;
    m.set_move(Square::A7, Square::A8, Piece::wB, move::Flag::NORMAL, move::Promotion::BISHOP);
    std::cout << square_str[static_cast<int>(m.get_from())] << std::endl;
    std::cout << square_str[static_cast<int>(m.get_to())] << std::endl;
    std::cout << static_cast<int>(m.get_piece()) << std::endl;
    std::cout << static_cast<int>(m.get_flag()) << std::endl;
    std::cout << static_cast<int>(m.get_promotion()) << std::endl;
    m.print_uci();
    return 0;
}