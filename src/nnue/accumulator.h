#pragma once

#include <array>

#include "../defs.h"
#include "../move.h"

#include "arch.h"
#include "weights.h"

namespace elixir {
    class Board;
}

namespace elixir::nnue {
    class Accumulator {
        public:
            void set_position(const Board& board, Network& net);

            Accumulator() = default;

            alignas(32) std::array<std::array<I16, HIDDEN_SIZE>, 2> accumulator;
            alignas(8) std::array<U8, 2> king_buckets;

            std::array<I16, HIDDEN_SIZE> &operator[](size_t i) { return accumulator[i]; }
            const std::array<I16, HIDDEN_SIZE> &operator[](size_t i) const { return accumulator[i]; }
            
            void add(const Piece piece, const Square sq, Network& net);
            void remove(const Piece piece, const Square sq, Network& net);
            void make_move(const Board& board, const move::Move& move, Network& net);
        };
}