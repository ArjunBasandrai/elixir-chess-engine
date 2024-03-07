#include <iostream>

#include "movepicker.h"
#include "move.h"
#include "movegen.h"
#include "evaluate.h"
#include "search.h"

namespace elixir {
 
    void MovePicker::score_moves(Board &board, move::Move tt_move, search::SearchStack *ss) {
        int value;
        Square from, to;
        int from_piece, to_piece;
        int from_val, to_val;
        for (int i = 0; i < moves.size(); i++)
        {

            move::Move move = moves[i];

            value = 0;

            // from Ethereal
            from = move.get_from();
            to = move.get_to();
            from_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(from)));
            to_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
            from_val = eval::piece_values[from_piece];
            to_val = eval::piece_values[to_piece];

            if (move == tt_move)
            {
                // (~20 ELO)
                value += 16384;
            }

            if (move == ss->killers[0])
            {
                value += 512;
            }
            else if (move == ss->killers[1])
            {
                value += 256;
            }

            value += 5 * to_val;

            if (move.is_en_passant())
            {
                value += 2 * eval::piece_values[static_cast<int>(PieceType::PAWN)];
            }
            else if (move.is_promotion() && move.get_promotion() == move::Promotion::QUEEN)
            {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::QUEEN)];
            }
            else if (move.is_castling())
            {
                value += 256;
            }
            scores[i] = value;
        }
    }

    // (~300 ELO)
    void MovePicker::sort_moves(Board &board, move::Move tt_move, search::SearchStack *ss) {
        scores.resize(moves.size());
        score_moves(board, tt_move, ss);

        for (int i = 0; i < moves.size(); i++) {
            for (int j = i + 1; j < moves.size(); j++) {
                if (scores[j] > scores[i]) {
                    std::swap(scores[i], scores[j]);
                    std::swap(moves[i], moves[j]);
                }
            }
        }
    }

    void MovePicker::init_mp(Board& board, move::Move tt_move, search::SearchStack *ss, bool for_qs) {
        if (for_qs)
        moves = movegen::generate_moves<true>(board);
        else
        moves = movegen::generate_moves<false>(board);
        sort_moves(board, tt_move, ss);
    }
}