#include <iostream>
#include <cassert>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"

namespace elixir::eval {
    bool is_material_draw(Board &board) {
        // K v K
        if (board.get_piece_count() == 2) {
            return true;
        } 
        // KN v K, KB v K
        else if (board.get_piece_count() == 3 && (bits::count_bits(board.knights()) == 1 || bits::count_bits(board.bishops()) == 1)) {
            return true;
        } 
        else if (board.get_piece_count() == 4) {
            // KNN v K, KN v KN
            if (bits::count_bits(board.knights()) == 2) {
                return true;            
            } 
            // KB v KB
            else if (bits::count_bits(board.bishops()) == 1 && bits::count_bits(board.white_bishops()) == 1) {
                return true;
            }
        }
        return false;
    }

    Phase get_game_phase(Board &board, int game_phase_score) {
        if (game_phase_score > opening_phase_score) {
            return Phase::OPENING;
        } else if (game_phase_score < endgame_phase_score) {
            return Phase::ENDGAME;
        } else {
            return Phase::MIDDLEGAME;
        }
    }

    Score interpolate_eval(Score opening_score, Score endgame_score, Board &board) {
        int game_phase_score = board.get_phase_score();
        Phase phase = get_game_phase(board, game_phase_score);
        if (phase == Phase::OPENING) {
            return opening_score;
        } else if (phase == Phase::ENDGAME) {
            return endgame_score;
        } else {
            return ((opening_score * game_phase_score +
                    endgame_score * (opening_phase_score - game_phase_score)) /
                    opening_phase_score);
        }
    }

    int evaluate(Board& board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();
        EvalScore eval = board.get_eval();
        score_opening = O(eval);
        score_endgame = E(eval);
        score = interpolate_eval(score_opening, score_endgame, board);
        return (side == Color::WHITE) ? score : -score;
    }
}