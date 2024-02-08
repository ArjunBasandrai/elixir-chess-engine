#include "perft.h"

#include <iostream>
#include <vector>
#include <chrono>

#include "../board/board.h"
#include "../utils/static_vector.h"
#include "../move.h"
#include "../movegen.h"

namespace elixir {
    void perft_driver(Board& board, int depth, long long& nodes) {
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

    void perft_test(Board& board, int depth, long long& nodes) {
        nodes = 0;
        StaticVector<move::Move, 256> moves = movegen::generate_moves(board);
        auto start = std::chrono::high_resolution_clock::now();
        for (auto m: moves) {

            if (!board.make_move(m)) {
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
        auto stop = std::chrono::high_resolution_clock::now();
        U64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        std::cout << "Depth: " << depth << " nodes: " << nodes << " time: " << duration << " ms nps: " << static_cast<int>((double)nodes * 1000 / (duration)) << std::endl;

    }
}