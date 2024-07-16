#pragma once

#include <array>
#include <string>

#include "../defs.h"
#include "../types.h"
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
            Accumulator(const Board& board);

            std::array<std::array<I16, HIDDEN_SIZE>, 2> accumulator;

            std::array<I16, HIDDEN_SIZE> &operator[](size_t i) { return accumulator[i]; }
            const std::array<I16, HIDDEN_SIZE> &operator[](size_t i) const { return accumulator[i]; }

        };
        
        inline Network net;

        class NNUE {
            public:
                void init(const std::string file);
                int eval(const Accumulator &accumulator, Color side);
        };
    }
    inline nnue::NNUE nn;
}