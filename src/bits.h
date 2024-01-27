#pragma once

#include <bit>
#include <cassert>

#include "types.h"
#include "defs.h"

namespace elixir {
    namespace bits {
        inline U64 bit(Square sq) { 
            assert(sq != Square::no_sq);
            return 1ULL << static_cast<int>(sq); 
        }

        inline int get_bit(U64 bb, Square sq) { 
            assert(sq != Square::no_sq && bb);
            return bb & bit(sq); 
        }

        inline void set_bit(U64 &bb, Square sq) { 
            assert(sq != Square::no_sq && bb);
            bb |= bit(sq); 
        }

        inline void clear_bit(U64 &bb, Square sq) { 
            assert(sq != Square::no_sq && bb);
            bb &= ~bit(sq); 
        }

        inline void flip_bit(U64 &bb, Square sq) { 
            assert(sq != Square::no_sq && bb);
            bb ^= bit(sq); 
        }

        inline int count_bits(U64 bb) { 
            return std::popcount(bb); 
        }

        inline int lsb_index(U64 bb) { 
            assert(bb);
            return std::countr_zero(bb); 
        }

        inline int msb_index(U64 bb) { 
            assert(bb);
            return 63 - std::countl_zero(bb); 
        }

        inline int pop_bit(U64 &bb) {
            assert(bb);
            int index = lsb_index(bb);
            clear_bit(bb, static_cast<Square>(index));
            return index;
        }
    }
    extern void print_bitboard(U64 bitboard);
}