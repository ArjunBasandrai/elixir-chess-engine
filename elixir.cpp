#include <string>

#include "src/types.h"
#include "src/defs.h"
#include "src/utils/test_fens.h"
#include "src/utils/masks.h"
#include "src/board/board.h"
#include "src/bench/bench.h"
#include "src/hashing/hash.h"
#include "src/attacks/attacks.h"
#include "src/tests/see_test.h"
#include "src/uci.h"
#include "src/search.h"
#include "src/tt.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
    search::init_lmr();
}

int main(int argc, char *argv[]) {
    init();

    if (argc > 1) {
        if (std::string(argv[1]) == "bench") {
            bench::bench();
            return 0;
        }
    }

    Board board;
    board.to_startpos();

    tests::see_test();

    // uci::uci_loop(board);
    return 0;
}