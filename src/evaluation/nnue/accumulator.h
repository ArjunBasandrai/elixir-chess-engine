#pragma once

#include <array>

#include "defs.h"
#include "move.h"

#include "arch.h"
#include "weights.h"

namespace elixir {
    // forward declaration
    class Board;
}

namespace elixir::nnue {
    class Accumulator {
      public:
        Accumulator() = default;

        alignas(64) std::array<std::array<I16, HIDDEN_SIZE>, 2> accumulator;

        std::array<I16, HIDDEN_SIZE> &operator[](size_t i) { return accumulator[i]; }
        const std::array<I16, HIDDEN_SIZE> &operator[](size_t i) const { return accumulator[i]; }

        void add(const Piece piece, const Square sq, Network &net);
        void remove(const Piece piece, const Square sq, Network &net);
    };
}