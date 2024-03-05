#include <iostream>
#include <cassert>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"

namespace elixir::eval {
    Phase get_game_phase_score(Board &board, int &game_phase_score) {
        int white_piece_scores = 0, black_piece_scores = 0;

        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::KNIGHT)) * O(material_score[1]);
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::BISHOP)) * O(material_score[2]);
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::ROOK)) * O(material_score[3]);
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::QUEEN)) * O(material_score[4]);

        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::KNIGHT)) * O(material_score[1]);
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::BISHOP)) * O(material_score[2]);
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::ROOK)) * O(material_score[3]);
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::QUEEN)) * O(material_score[4]);

        game_phase_score = white_piece_scores + black_piece_scores;

        if (game_phase_score > opening_phase_score) {
            return Phase::OPENING;
        } else if (game_phase_score < endgame_phase_score) {
            return Phase::ENDGAME;
        } else {
            return Phase::MIDDLEGAME;
        }
    }

    Score interpolate_eval(Score opening_score, Score endgame_score, Board &board) {
        int game_phase_score = 0;
        Phase phase = get_game_phase_score(board, game_phase_score);
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