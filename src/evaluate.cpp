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

    EvalScore base_eval(Board& board) {
        Score score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();
        if (board.has_undo_state()) {
            EvalScore eval = board.get_last_eval();
            score_opening = O(eval);
            score_endgame = E(eval);
            auto move = board.get_last_move();
            
            if (move.is_normal() || move.is_double_pawn_push()) {
                auto from = static_cast<int>(move.get_from());
                auto to = static_cast<int>(move.get_to());
                const auto piece = move.get_piece();
                const auto piecetype = static_cast<int>(board.piece_to_piecetype(piece));
                const int color = static_cast<int>(side)^1;
                if (color == static_cast<I8>(Color::WHITE)) { from ^= 0b111000; to ^= 0b111000; }
                score_opening += (O(eval::psqt[piecetype][to]) - O(psqt[piecetype][from])) * color_offset[color];
                score_endgame += (E(eval::psqt[piecetype][to]) - E(psqt[piecetype][from])) * color_offset[color];
                return S(score_opening, score_endgame);                         
            } else if (move.is_castling()) {
                auto from = static_cast<int>(move.get_from());
                auto to = static_cast<int>(move.get_to());
                const auto piece = move.get_piece();
                const auto piecetype = static_cast<int>(board.piece_to_piecetype(piece));
                assert(piecetype == static_cast<int>(PieceType::KING));
                const int color = static_cast<int>(side)^1;
                int from_rook_square;
                int to_rook_square;
                switch (static_cast<Square>(to)) {
                    case Square::C1:
                        from_rook_square = static_cast<int>(Square::A1);
                        to_rook_square = static_cast<int>(Square::D1);
                        break;
                    case Square::G1:
                        from_rook_square = static_cast<int>(Square::H1);
                        to_rook_square = static_cast<int>(Square::F1);
                        break;
                    case Square::C8:
                        from_rook_square = static_cast<int>(Square::A8);
                        to_rook_square = static_cast<int>(Square::D8);
                        break;
                    case Square::G8:
                        from_rook_square = static_cast<int>(Square::H8);
                        to_rook_square = static_cast<int>(Square::F8);
                        break;
                    default:
                        assert(false);
                        break;
                }
                if (color == static_cast<I8>(Color::WHITE)) { from ^= 0b111000; to ^= 0b111000; from_rook_square ^= 0b111000; to_rook_square ^= 0b111000; }
                score_opening += (O(eval::psqt[piecetype][to]) - O(psqt[piecetype][from])) * color_offset[color];
                score_endgame += (E(eval::psqt[piecetype][to]) - E(psqt[piecetype][from])) * color_offset[color];
                score_opening += (O(eval::psqt[static_cast<int>(PieceType::ROOK)][to_rook_square]) - O(psqt[static_cast<int>(PieceType::ROOK)][from_rook_square])) * color_offset[color];
                score_endgame += (E(eval::psqt[static_cast<int>(PieceType::ROOK)][to_rook_square]) - E(psqt[static_cast<int>(PieceType::ROOK)][from_rook_square])) * color_offset[color];
                return S(score_opening, score_endgame);
            }
        }
        score_opening = 0;
        score_endgame = 0;
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
        return S(score_opening, score_endgame);
    }

    int evaluate(Board& board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();
        EvalScore eval = base_eval(board);
        score_opening = O(eval);
        score_endgame = E(eval);
        score = interpolate_eval(score_opening, score_endgame, board);
        return (side == Color::WHITE) ? score : -score;
    }
}