#pragma once

#include <bit>
#include <cassert>
#include <iostream>

#include "../defs.h"
#include "../types.h"

namespace elixir {
    namespace bits {
        inline Bitboard bit(const Square sq) {

            assert(sq != Square::NO_SQ);

            return 1ULL << static_cast<int>(sq);
        }

        [[nodiscard]] inline Bitboard get_bit(Bitboard bb, const Square sq) {

            assert(sq != Square::NO_SQ);

            return bb & bit(sq);
        }

        inline void set_bit(Bitboard &bb, const Square sq) {

            assert(sq != Square::NO_SQ);

            bb |= bit(sq);
        }

        inline void clear_bit(Bitboard &bb, const Square sq) {

            assert(sq != Square::NO_SQ && bb && get_bit(bb, sq));

            bb &= ~bit(sq);
        }

        inline void flip_bit(Bitboard &bb, const Square sq) {

            assert(sq != Square::NO_SQ);

            bb ^= bit(sq);
        }

        [[nodiscard]] inline int count_bits(const Bitboard bb) {
            return std::popcount(bb);
        }

        [[nodiscard]] inline int lsb_index(const Bitboard bb) {

            assert(bb);

            return std::countr_zero(bb);
        }

        [[nodiscard]] inline int msb_index(const Bitboard bb) {

            assert(bb);

            return 63 - std::countl_zero(bb);
        }

        inline int pop_bit(Bitboard &bb) {

            assert(bb);

            int index = lsb_index(bb);
            clear_bit(bb, static_cast<Square>(index));
            return index;
        }

        static inline Square relative_last_sq(const Bitboard bb, const Color side) {
            return static_cast<Square>((side == Color::WHITE) ? lsb_index(bb) : msb_index(bb));
        }
    }
    extern void print_bitboard(const Bitboard bitboard) noexcept;
}