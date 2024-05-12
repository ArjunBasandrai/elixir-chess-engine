#include <iostream>

#include "movepicker.h"
#include "move.h"
#include "movegen.h"
#include "evaluate.h"
#include "search.h"

namespace elixir {
 
    void MovePicker::score_moves(const Board &board, const move::Move& tt_move, const search::SearchStack *ss) {
        scores.resize(moves.size());

        int value;
        Square from, to;
        int from_piece, to_piece;
        int from_val, to_val;

        for (int i = 0; i < moves.size(); i++){

            move::Move move = moves[i];

            value = 0;

            // from Ethereal
            from = move.get_from();
            to = move.get_to();
            from_piece = static_cast<int>(board.piece_to_piecetype(move.get_piece()));
            to_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
            from_val = eval::piece_values[from_piece];
            to_val = eval::piece_values[to_piece];

            // (~20 ELO)
            if (move == tt_move) { value += 16384; }

            if (move == ss->killers[0]) { value += 512; }
            else if (move == ss->killers[1]) { value += 256; }

            value += 5 * to_val;

            if (move.is_en_passant()) { value += 2 * eval::piece_values[static_cast<int>(PieceType::PAWN)]; }
            else if (move.is_promotion() && move.get_promotion() == move::Promotion::QUEEN) { value += 5 * eval::piece_values[static_cast<int>(PieceType::QUEEN)]; }
            else if (move.is_castling()) { value += 256; }

            // (~45 ELO)
            value += board.history[static_cast<int>(from)][static_cast<int>(to)];

            scores[i] = value;
        }
    }


    void MovePicker::init_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool for_qs) {
        if (for_qs)
        moves = movegen::generate_moves<true>(board);
        else
        moves = movegen::generate_moves<false>(board);
        score_moves(board, tt_move, ss);
    }

    move::Move MovePicker::next_move() {
        if (moves.size() <= 0) return move::NO_MOVE;

        // (~300 ELO)
        int max = -INF, max_idx = 0;
        for (int i = 0; i < moves.size(); i++) {
            if (scores[i] > max) {
                max = scores[i];
                max_idx = i;
            }
        }

        auto best_move = moves[max_idx];

        int last_index = moves.size() - 1;
        std::swap(moves[max_idx], moves[last_index]);
        std::swap(scores[max_idx], scores[last_index]);

        moves.pop_back();
        scores.pop_back();

        return best_move;
    }
}