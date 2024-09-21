#include <climits>
#include <iostream>

#include "evaluation/evaluate.h"
#include "move.h"
#include "movegen.h"
#include "movepicker.h"
#include "search.h"
#include "ss.h"

namespace elixir {

    int MP_SEE = 117;

    void MovePicker::score_moves(const Board &board, const move::Move &tt_move,
                                 const search::SearchStack *ss, const History &history) {
        scores.resize(moves.size());

        int value;
        Square from, to;

        for (int i = 0; i < moves.size(); i++) {

            move::Move move = moves[i];

            value = 0;

            from = move.get_from();
            to   = move.get_to();

            // TT Move Ordering(~180 ELO)
            if (move == tt_move) {
                value = INT_MAX;
            }
            // Move Ordering (~450 ELO)
            else if (move.is_promotion()) {
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
                auto captured_piece =
                    move.is_en_passant() ? static_cast<int>(PieceType::PAWN)
                                         : static_cast<int>(piece_to_piecetype(board.piece_on(to)));
                value = eval::piece_values[captured_piece];
                value += search::SEE(board, move, -MP_SEE) ? 1000000000 : -1000000;
            } else if (move == ss->killers[0]) {
                value = 800000000;
            } else if (move == ss->killers[1]) {
                value = 700000000;
            } else if (move ==
                       history.countermove_history.get_countermove(board.get_side_to_move(),
                                                                   (ss - 1)->move.get_from(),
                                                                   (ss - 1)->move.get_to())) {
                value = 600000000;
            } else {
                value = history.get_history(move, ss);
            }

            scores[i] = value;
        }

        const int n = scores.size();
        for (int i = 1; i < n; i++) {
            int key_score = scores[i];
            auto key_move = moves[i];
            
            int j = i - 1;
            
            while (j >= 0 && scores[j] > key_score) {
                scores[j + 1] = scores[j];
                moves[j + 1] = moves[j];
                j--;
            }
            
            scores[j + 1] = key_score;
            moves[j + 1] = key_move;
        }
    }

    void MovePicker::init_mp(const Board &board, move::Move tt_move, search::SearchStack *ss,
                             History &history, bool for_qs) {
        if (for_qs)
            moves = movegen::generate_moves<true>(board);
        else
            moves = movegen::generate_moves<false>(board);
        score_moves(board, tt_move, ss, history);
    }

    move::Move MovePicker::next_move() {
        if (moves.size() <= 0)
            return move::NO_MOVE;

        auto best_move = moves[moves.size() - 1];

        moves.pop_back();
        scores.pop_back();

        return best_move;
    }
}