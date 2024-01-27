#include <iostream>
#include <cstdlib>

#include "src/types.h"
#include "src/defs.h"
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
    board.print_board();

    return 0;
}