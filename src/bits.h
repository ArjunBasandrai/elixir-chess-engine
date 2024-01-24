#pragma once

#include "defs.h"

#define bit(sq) (1ULL << (sq))
#define get_bit(bb, sq) (bb & bit(sq))
#define set_bit(bb, sq) (bb |= bit(sq))
#define pop_bit(bb, sq) (bb &= ~bit(sq))
#define flip_bit(bb, sq) (bb ^= bit(sq))

#define count_bits(bb) __builtin_popcountll(bb)
#define lsb_index(bb) (__builtin_ctzll(bb))
#define msb_index(bb) (63 ^ __builtin_clzll(bb))

void print_bitboard(U64 bitboard);