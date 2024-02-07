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
    board.from_fen("rnbqkbnr/pppppppp/8/K3P2r/8/8/PPPPPPPP/RNBQ1BNR w KQkq - 0 1 ");
    board.print_board();
    board.parse_uci_move("a5a6");
    board.print_board();

    return 0;
}