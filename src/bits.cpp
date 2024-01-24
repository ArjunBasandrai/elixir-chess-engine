#include <iostream>

#include "bits.h"

void print_bitboard(U64 bitboard) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (bitboard & bit(square)) {
                std::cout << "1 ";
            } else {
                std::cout << "- ";
            }
        }
        std::cout << "\n";
    }
}