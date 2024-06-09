#include "perft.h"

#include <chrono>
#include <iostream>
#include <vector>

#include "../board/board.h"
#include "../move.h"
#include "../movegen.h"
#include "../utils/static_vector.h"

namespace elixir {
    void perft_driver(Board &board, int depth, long long &nodes) {
        if (! depth) {
            nodes++;
            return;
        }
        MoveList moves = movegen::generate_moves<false>(board);
        for (auto m : moves) {
            if (! board.make_move(m)) {
                continue;
            }

            perft_driver(board, depth - 1, nodes);
            board.unmake_move(m, true);
        }
    }

    void perft_test(Board &board, int depth, long long &nodes) {
        nodes          = 0;
        MoveList moves = movegen::generate_moves<false>(board);
        auto start     = std::chrono::high_resolution_clock::now();
        for (auto m : moves) {

            if (! board.make_move(m)) {
                continue;
            }

            long long cummulative_nodes = nodes;
            perft_driver(board, depth - 1, nodes);
            long long old_nodes = nodes - cummulative_nodes;

            board.unmake_move(m, true);

            std::cout << "moves: ";
            m.print_uci();
            std::cout << " nodes: " << old_nodes << std::endl;
        }
        auto stop    = std::chrono::high_resolution_clock::now();
        U64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        std::cout << "Depth: " << depth << " nodes: " << nodes << " time: " << duration
                  << " ms nps: " << static_cast<int>((double)nodes * 1000 / (duration))
                  << std::endl;
    }
}