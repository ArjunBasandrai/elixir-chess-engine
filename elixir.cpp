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

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
}

int main() {
    init();

    Board board;
    board.from_fen(start_position);
    uci::uci_loop(board);
    return 0;
}