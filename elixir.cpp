#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "src/types.h"
#include "src/defs.h"
#include "src/utils/test_fens.h"
#include "src/board/board.h"
#include "src/hashing/hash.h"
#include "src/attacks/attacks.h"
#include "src/uci.h"
#include "src/evaluate.h"
#include "src/search.h"
#include "src/movegen.h"
#include "src/utils/perft.h"
#include "src/tt.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
}

int main() {
    init();
    TranspositionTable tt(64);
    move::Move m1;
    move::Move m2;

    m1.set_move(Square::E2, Square::E4, Piece::wP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
    m2.set_move(Square::E7, Square::E5, Piece::bP, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);

    Board board;
    board.to_startpos();

    board.make_move(m1);
    U64 hash1 = board.get_hash_key();
    std::cout << "Hash 1: " << hash1 << std::endl;
    tt.store_tt(hash1, 50, m1, 6, TTFlag::TT_EXACT);
    
    board.make_move(m2);
    U64 hash2 = board.get_hash_key();
    std::cout << "hash 2: " << hash2 << std::endl;
    tt.store_tt(hash2, 1600, m2, 8, TTFlag::TT_ALPHA);

    ProbedEntry entry1 = tt.probe_tt(hash1);
    ProbedEntry entry2 = tt.probe_tt(hash2);

    std::cout << entry1.score << std::endl;
    std::cout << entry2.score << std::endl;

    entry1.best_move.print_uci();
    std::cout << std::endl;
    entry2.best_move.print_uci();
    std::cout << std::endl;

    std::cout << "Depth: " << static_cast<int>(entry1.depth) << std::endl;
    std::cout << "Depth: " << static_cast<int>(entry2.depth) << std::endl;

    std::cout << static_cast<int>(entry1.flag) << std::endl;
    std::cout << static_cast<int>(entry2.flag) << std::endl;

    return 0;
}