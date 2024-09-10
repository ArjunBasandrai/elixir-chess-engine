#pragma once

#include <iostream>
#include <array>

#include "move.h"

namespace elixir::search {
    struct PVariation {
        std::size_t length;
        std::array<move::Move, 256> line;

        PVariation() : length(0) {
            for (auto &move : line) {
                move = move::NO_MOVE;
            }
        }

        void print_pv() const {
            for (int i = 0; i < length; i++) {
                line[i].print_uci();
                std::cout << " ";
            }
        }

        void update(const move::Move m, const PVariation &rest) {
            line[0] = m;
            std::copy(rest.line.begin(), rest.line.begin() + rest.length, line.begin() + 1);
            length = rest.length + 1;
        }
    };
}