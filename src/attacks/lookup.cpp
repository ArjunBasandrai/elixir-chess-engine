#include "lookup.h"

#include "../defs.h"
#include "../utils/bits.h"

namespace elixir::attacks {
    Bitboard pawn_attacks[2][64];
    Bitboard mask_pawn_attacks(Square sq, Color side) {
        Bitboard bb = 0ULL;
        Bitboard mask = 0ULL;
        bits::set_bit(bb, sq);
        int rank = (static_cast<int>(sq) >> 3) & 7;
        int file = static_cast<int>(sq) & 7;
        switch (side) {
            case Color::WHITE:
                mask |= (bb << 7) & not_h_file;
                mask |= (bb << 9) & not_a_file;
                break;
            case Color::BLACK:
                mask |= (bb >> 7) & not_a_file;
                mask |= (bb >> 9) & not_h_file;
                break;
            default:
                break;
        }
        return mask;
    }

    void init_pawn_attacks() {
        for (int i = 0; i < 64; i++) {
            pawn_attacks[static_cast<int>(Color::WHITE)][i] = mask_pawn_attacks(static_cast<Square>(i), Color::WHITE);
            pawn_attacks[static_cast<int>(Color::BLACK)][i] = mask_pawn_attacks(static_cast<Square>(i), Color::BLACK);
        }
    }
}