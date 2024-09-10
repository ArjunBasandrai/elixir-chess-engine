#include <iostream>
#include <string>

#include "attacks/attacks.h"
#include "search/bench/bench.h"
#include "board/board.h"
#include "defs.h"
#include "hashing/hash.h"
#include "search/search.h"
#include "src/spsa/spsa.h"
#include "src/tests/see_test.h"
#include "src/texel/texel.h"
#include "uci.h"

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