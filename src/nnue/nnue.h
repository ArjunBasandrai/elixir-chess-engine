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
        inline Network net;
        class NNUE {
            public:
                std::vector<Accumulator> accumulators;
                int current_acc;

                void reset() {
                    current_acc = 0;
                    accumulators[current_acc] = Accumulator();
                }

                NNUE() {
                    accumulators.resize(1024);
                    reset();
                }

                // copy constructor
                NNUE(const NNUE& nnue) {
                    accumulators = nnue.accumulators;
                    current_acc = nnue.current_acc;
                }

                // copy assignment
                NNUE& operator=(const NNUE& nnue) {
                    accumulators = nnue.accumulators;
                    current_acc = nnue.current_acc;
                    return *this;
                }

                void init(const std::string file);
                void set_position(const Board &board);

                void refresh(const Board &board) {
                    reset();
                    set_position(board);
                }

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
                    increment_acc();
                    accumulators[current_acc].make_move(board, move, net);
                }

                auto get_king_bucket(const Color side, const Square king_sq) {
                    const U8 sq = static_cast<U8>(king_sq);
                    const U8 color = static_cast<U8>(side);
                    return KING_BUCKET_SCHEME[sq ^ (56 * color)];
                }

                bool needs_refresh(const int previous_bucket, const Color side, const Square king_sq) {
                    return get_king_bucket(side, king_sq) != previous_bucket;
                }

                int eval(const Color side, const int bucket);
        };
    }
}