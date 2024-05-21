#include <iostream>
#include <limits>

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

        for (int i = 0; i < moves.size(); i++){

            move::Move move = moves[i];

            value = 0;

            // from Ethereal
            from = move.get_from();
            to = move.get_to();

            // (~20 ELO)
            if (move == tt_move) { 
                value = INT_MAX; 
            } else if (move.is_promotion()) {
                switch (move.get_promotion()) {
                    case move::Promotion::QUEEN:
                        value = 2000000001;
                        break;
                    case move::Promotion::KNIGHT:
                        value = 2000000000;
                        break;
                    case move::Promotion::ROOK:
                        value = -2000000001;
                        break;
                    case move::Promotion::BISHOP:
                        value = -2000000000;
                        break;
                    default:
                        break;
                }
            } else if (move.is_capture() || move.is_en_passant()) {
                auto captured_piece = move.is_en_passant() ? static_cast<int>(PieceType::PAWN) : static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
                value = eval::piece_values[captured_piece];
                value += 1000000000;
            } else if (move == ss->killers[0]) { 
                value = 800000000; 
            } else if (move == ss->killers[1]) { 
                value = 700000000; 
            } else {
                // Butterfly History Move Ordering (~45 ELO)
                value = board.history[static_cast<int>(from)][static_cast<int>(to)];
            }

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