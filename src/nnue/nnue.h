#pragma once

#include <array>
#include <string>
#include <vector>

#include "../defs.h"
#include "../types.h"
#include "../move.h"
#include "../board/board.h"
#include "incbin.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE   = 32;
constexpr int SCALE = 400;
constexpr int L1Q = 255;
constexpr int OutputQ = 64;

namespace elixir {
    namespace nnue {
        struct Network {
            I16 layer_1_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
            I16 layer_1_biases[HIDDEN_SIZE];
            I16 output_weights[2][HIDDEN_SIZE];
            I16 output_bias;
        };

        class Accumulator {
        public:
            void set_position(const Board& board);

            Accumulator() = default;
            Accumulator(const Board& board) {
                set_position(board);
            }

            std::array<std::array<I16, HIDDEN_SIZE>, 2> accumulator;

            std::array<I16, HIDDEN_SIZE> &operator[](size_t i) { return accumulator[i]; }
            const std::array<I16, HIDDEN_SIZE> &operator[](size_t i) const { return accumulator[i]; }
            
            void add(const Piece piece, const Square sq);
            void remove(const Piece piece, const Square sq);
            void make_move(const Board& board, const move::Move& move);
        };
        
        inline Network net;

        class NNUE {
            public:
                std::vector<Accumulator> accumulators;
                int current_acc;

                void reset() {
                    accumulators.clear();
                    current_acc = 0;
                }

                NNUE() {
                    accumulators.reserve(512);
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

                int eval(const Color side);
        };
    }
    inline nnue::NNUE nn;
}