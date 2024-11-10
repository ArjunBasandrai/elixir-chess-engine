#pragma once

#include <array>
#include <string>
#include <vector>

#include "defs.h"
#include "move.h"
#include "types.h"

#include "accumulator.h"
#include "arch.h"
#include "weights.h"

namespace elixir {
    class Board;
    namespace nnue {
        inline Network net;
        class NNUE {
          public:
            std::vector<Accumulator> accumulators;
            int current_acc;

            void reset() {
                current_acc               = 0;
                accumulators[current_acc] = Accumulator();
            }

            NNUE() {
                accumulators.resize(1024);
                reset();
            }

            NNUE(const NNUE &nnue) {
                accumulators = nnue.accumulators;
                current_acc  = nnue.current_acc;
            }

            NNUE &operator=(const NNUE &nnue) {
                accumulators = nnue.accumulators;
                current_acc  = nnue.current_acc;
                return *this;
            }

            void init(const std::string file);

            void refresh() {
                reset();
                for (int i = 0; i < HIDDEN_SIZE; i++) {
                    accumulators[current_acc][0][i] = net.layer_1_biases[i];
                }

                for (int i = 0; i < HIDDEN_SIZE; i++) {
                    accumulators[current_acc][1][i] = net.layer_1_biases[i];
                }
            }

            void increment_acc() {
                current_acc++;
                accumulators[current_acc] = accumulators[current_acc - 1];
            }

            void decrement_acc() { current_acc--; }

            void add(const Piece piece, const Square square) {
                accumulators[current_acc].add(piece, square, net);
            }

            void sub(const Piece piece, const Square square) {
                accumulators[current_acc].remove(piece, square, net);
            }

            void add_sub(const Piece piece1, const Square sq1, const Piece piece2, const Square sq2) {
                accumulators[current_acc].add_sub(piece1, sq1, piece2, sq2, net);
            }

            void add_sub_sub(const Piece piece1, const Square sq1, const Piece piece2, const Square sq2,
                             const Piece piece3, const Square sq3) {
                accumulators[current_acc].add_sub_sub(piece1, sq1, piece2, sq2, piece3, sq3, net);
            }

            void add_add_sub_sub(const Piece piece1, const Square sq1, const Piece piece2, const Square sq2,
                                 const Piece piece3, const Square sq3, const Piece piece4, const Square sq4) {
                accumulators[current_acc].add_add_sub_sub(piece1, sq1, piece2, sq2, piece3, sq3, piece4, sq4, net);
            }

            Accumulator &get_acc() { return accumulators[current_acc]; }

            int eval(const Color side, const int bucket);
        };
    }
}