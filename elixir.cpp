#include <iostream>
#include <cstdlib>
#include <vector>

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

    Board board;
    board.from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
    board.print_board();

    getchar();

    move::Move m, m2, m3, m4, m5, m6;
    m.set_move(Square::E2, Square::E4, Piece::wP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    m2.set_move(Square::D7, Square::D5, Piece::bP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    m3.set_move(Square::E4, Square::E5, Piece::wP, move::Flag::NORMAL, move::Promotion::QUEEN);
    m4.set_move(Square::D5, Square::D4, Piece::bP, move::Flag::NORMAL, move::Promotion::QUEEN);
    m5.set_move(Square::C2, Square::C4, Piece::wP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    m6.set_move(Square::D4, Square::C3, Piece::bP, move::Flag::EN_PASSANT, move::Promotion::QUEEN);

    board.make_move(m);
    board.print_board();
    getchar();

    board.make_move(m2);
    board.print_board();
    getchar();

    board.make_move(m3);
    board.print_board();
    getchar();

    board.make_move(m4);
    board.print_board();
    getchar();

    board.make_move(m5);
    board.print_board();
    getchar();

    board.make_move(m6);
    board.print_board();
    getchar();
    
    board.unmake_move(m6, true);
    board.print_board();
    getchar();
    
    board.unmake_move(m5, true);
    board.print_board();
    getchar();
    
    board.unmake_move(m4, true);
    board.print_board();
    getchar();
    
    board.unmake_move(m3, true);
    board.print_board();
    getchar();
    
    board.unmake_move(m2, true);
    board.print_board();
    getchar();
    
    board.unmake_move(m, true);
    board.print_board();
    getchar();

    // std::vector<int> list;
    // list.reserve(100);
    // list.emplace_back(6);
    // list.emplace_back(150);
    // list.emplace_back(7);
    // std::cout << list[0] << std::endl;
    // std::cout << list.size() << std::endl;
    // std::cout << list.back() << std::endl;
    // list.pop_back();
    // std::cout << list.size() << std::endl;
    // std::cout << list.back() << std::endl;

    return 0;
}