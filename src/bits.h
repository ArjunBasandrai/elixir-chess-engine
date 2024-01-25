#pragma once

#include <bit>
#include <cassert>

#include "defs.h"

#define bit(sq) (1ULL << (sq))
#define get_bit(bb, sq) (bb & bit(sq))
#define set_bit(bb, sq) (bb |= bit(sq))
#define pop_bit(bb, sq) (bb &= ~bit(sq))
#define flip_bit(bb, sq) (bb ^= bit(sq))

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

void print_bitboard(U64 bitboard);