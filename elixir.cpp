#include <fstream>
#include <iostream>
#include <string>

#include "src/attacks/attacks.h"
#include "src/bench/bench.h"
#include "src/board/board.h"
#include "src/defs.h"
#include "src/hashing/hash.h"
#include "src/search.h"
#include "src/spsa.h"
#include "src/tests/see_test.h"
#include "src/texel/texel.h"
#include "src/tt.h"
#include "src/types.h"
#include "src/uci.h"
#include "src/utils/masks.h"
#include "src/utils/test_fens.h"

using namespace elixir;

void init() {
    zobrist::init_hash_keys();
    attacks::init_attacks();
    // magic::init_magic_numbers();
    search::init_lmr();
#ifdef USE_TUNE
    tune::init_tune();
#endif
}

int main(int argc, char *argv[]) {
    init();

    if (argc > 1) {
        if (std::string(argv[1]) == "bench") {
            bench::bench();
            return 0;
        }
        if (std::string(argv[1]) == "see") {
            tests::see_test();
            return 0;
        }
        if (std::string(argv[1]) == "spsa") {
#ifdef USE_TUNE
            tune::tuner.print_spsa_inputs();
            return 0;
#endif
        }
        if (std::string(argv[1]) == "texel") {
#ifdef TEXEL
            texel::texel.tune();
#endif
            return 0;
        }
    }

    Board board;
    board.to_startpos();

    uci::uci_loop(board);
    return 0;
}