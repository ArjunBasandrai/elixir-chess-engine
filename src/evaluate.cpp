#include <iostream>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"

namespace elixir::eval {
    int evaluate(Board& board) {
        int score = 0;
        Color side = board.get_side_to_move();
        for (int i = 0; i < 6; i++) {
            for (int color = 0; color < 2; color++) {
                Bitboard side_occ = board.color_occupancy(color);
                Bitboard piece_bb = board.piece_bitboard(static_cast<PieceType>(i)) & side_occ;
                while (piece_bb) {
                    int square = bits::pop_bit(piece_bb);
                    score += piece_values[i] * color_offset[color]; // + piece_square_table[i][square];
                }
            }
        }
        return (side == Color::WHITE) ? score : -score;
    }
}