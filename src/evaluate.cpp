#include <iostream>

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
        for (int i = 0; i < 6; i++) {
            for (int color = 0; color < 2; color++) {
                Bitboard side_occ = board.color_occupancy(color);
                Bitboard piece_bb = board.piece_bitboard(static_cast<PieceType>(i)) & side_occ;
                while (piece_bb) {
                    int square = bits::pop_bit(piece_bb);
                    if (color == static_cast<I8>(Color::WHITE)) { square ^= 0b111000; }
                    score_opening += (O(material_score[i]) + O(psqt[i][square])) * color_offset[color];
                    score_endgame += (E(material_score[i]) + E(psqt[i][square])) * color_offset[color];
                }
            }
        }
        score = interpolate_eval(score_opening, score_endgame, board);
        return (side == Color::WHITE) ? score : -score;
    }
}