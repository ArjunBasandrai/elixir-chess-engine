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
    
    Board board;
    board.from_fen(start_position);

    move::Move m;
    m.set_move(Square::E2, Square::E4, Piece::wP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    m.print_uci();
    std::cout << std::endl;

    move::Move b;
    b.set_move(Square::E7, Square::E5, Piece::bP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    b.print_uci();
    std::cout << std::endl;

    board.print_board();
    board.make_move(m);
    board.print_board();
    board.make_move(b);
    board.print_board();

    return 0;
}