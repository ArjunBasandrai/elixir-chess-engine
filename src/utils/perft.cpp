#include "perft.h"

#include <iostream>
#include <vector>

#include "../board/board.h"
#include "../utils/static_vector.h"
#include "../move.h"
#include "../movegen.h"

namespace elixir {
    void perft_driver(Board board, int depth, long& nodes) {
        if (!depth) {
            nodes++;
            return;
        }
        StaticVector<move::Move, 256> moves = movegen::generate_moves(board);
        for (auto m: moves) {
            if (!board.make_move(m)) {
                continue;
            }

            perft_driver(board, depth - 1, nodes);
            board.unmake_move(m, true);
        }
    }
}