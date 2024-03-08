#include <iostream>
#include <cassert>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"
#include "utils/eval_masks.h"

namespace elixir::eval {

    inline bool is_isolated_pawn(Bitboard friendlies, int file) {
        return !(friendlies & isolated_pawn_mask[file]);
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

    void evaluate_pawns(Board& board, Color side, EvalScore& eval) {
        Bitboard pawns = side == Color::WHITE ? board.white_pawns() : board.black_pawns();
        Bitboard friendly_pawns = pawns;
        Score score_opening = O(eval);
        Score score_endgame = E(eval);
        while (pawns) {
            int square = bits::pop_bit(pawns);
            int file = get_file(static_cast<Square>(square));

            if (is_isolated_pawn(friendly_pawns, file)) {
                score_opening -= O(pawn_isolated[file]);
                score_endgame -= E(pawn_isolated[file]);
            }
        }

        eval = S(score_opening, score_endgame);
    }

    int evaluate(Board& board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();
        EvalScore eval = board.get_eval();
        evaluate_pawns(board, side, eval);
        score_opening = O(eval);
        score_endgame = E(eval);
        score = interpolate_eval(score_opening, score_endgame, board);
        return (side == Color::WHITE) ? score : -score;
    }
}