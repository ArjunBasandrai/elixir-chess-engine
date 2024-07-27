#pragma once

#include <array>
#include <string>
#include <vector>

#include "../defs.h"
#include "../types.h"
#include "../move.h"

#include "incbin.h"
#include "arch.h"
#include "accumulator.h"
#include "weights.h"

namespace elixir {
    class Board;
    namespace nnue {
        class NNUE {
            public:
                Network net;
                std::vector<Accumulator> accumulators;
                int current_acc;

                void reset() {
                    accumulators.clear();
                    current_acc = 0;
                }

                NNUE() {
                    accumulators.reserve(1024);
                    reset();
                }

                void init(const std::string file);
                void set_position(const Board &board);

                void increment_acc() {
                    current_acc++;
                    accumulators[current_acc] = accumulators[current_acc - 1];
                }

                void decrement_acc() {
                    current_acc--;
                }

                Accumulator& get_acc() {
                    return accumulators[current_acc];
                }

                void make_move(const Board& board, const move::Move& move) {
                    accumulators[current_acc].make_move(board, move, net);
                }

                int eval(const Color side);
        };
    }
}