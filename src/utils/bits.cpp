#include <iostream>

#include "bits.h"

#include "../defs.h"
#include "../types.h"

namespace elixir {
    void print_bitboard(const Bitboard bitboard) noexcept {
        for (int rank = 7; rank >= 0; rank--) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if (! file) {
                    std::cout << rank + 1 << "   ";
                }
                if (bitboard & bits::bit(static_cast<Square>(square))) {
                    std::cout << "1 ";
                } else {
                    std::cout << "- ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        std::cout << "    a b c d e f g h\n\n";
        std::cout << "    Bitboard: " << bitboard << "ULL\n\n";
    }
}