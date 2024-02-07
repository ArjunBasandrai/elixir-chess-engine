#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>

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
#include "src/utils/perft.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
}

int main() {
    init();
    Board board;
    long nodes_master = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int depth = 1; depth <= 6; depth++) {
        board.from_fen(start_position);
        long nodes = 0;
        perft_driver(board, depth, nodes);    
        nodes_master += nodes;
    }
    auto stop = std::chrono::high_resolution_clock::now();
    U64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "nps: " << static_cast<int>((double)nodes_master * 1000 / (duration)) << std::endl;


    return 0;
}