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
#include "src/movegen.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
}

int main() {
    init();

    Board board;
    board.from_fen("rnbqkbnr/pppppppp/8/8/5r2/8/PPPPP1PP/RNBQK2R w KQkq - 0 1 ");
    board.print_board();

    std::vector<move::Move> move_list = movegen::generate_moves(board);
    std::cout << "Move list size: " << move_list.size() << std::endl;
    for (auto m : move_list) {
        getchar();
        if (board.make_move(m)) {
            board.print_board();
            board.unmake_move(m, true);
        } else {
            std::cout << "Invalid move :";
            m.print_uci();
            std::cout << "On :" << std::endl;
            board.print_board();
        }
    }
    board.print_board();

    return 0;
}