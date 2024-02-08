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
        std::cout << "Go" << std::endl;
        Bitboard side_occ = board.color_occupancy(side);
        for (int i = 0; i < 5; i++) {
            Bitboard piece_bb = board.piece_bitboard(static_cast<PieceType>(i)) & side_occ;
            while (piece_bb) {
                int square = bits::pop_bit(piece_bb);
                score += piece_values[i]; // + piece_square_table[i][square];
            }
        }
        return (side == Color::WHITE) ? score : -score;
    }
}