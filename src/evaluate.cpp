#include <iostream>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"

namespace elixir::eval
{
    Phase get_game_phase_score(Board &board, int &game_phase_score)
    {
        int white_piece_scores = 0, black_piece_scores = 0;

        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::KNIGHT)) * material_score[OPENING][1];
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::BISHOP)) * material_score[OPENING][2];
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::ROOK)) * material_score[OPENING][3];
        white_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::QUEEN)) * material_score[OPENING][4];

        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::KNIGHT)) * material_score[OPENING][1];
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::BISHOP)) * material_score[OPENING][2];
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::ROOK)) * material_score[OPENING][3];
        black_piece_scores += bits::count_bits(board.piece_bitboard(PieceType::QUEEN)) * material_score[OPENING][4];

        game_phase_score = white_piece_scores + black_piece_scores;

        if (game_phase_score > opening_phase_score)
        {
            return Phase::OPENING;
        }
        else if (game_phase_score < endgame_phase_score)
        {
            return Phase::ENDGAME;
        }
        else
        {
            return Phase::MIDDLEGAME;
        }
    }

    void get_material_and_positional_score(Board &board, int &score_opening, int &score_endgame)
    {
        for (int i = 0; i < 6; i++)
        {
            for (int color = 0; color < 2; color++)
            {
                Bitboard side_occ = board.color_occupancy(color);
                Bitboard piece_bb = board.piece_bitboard(static_cast<PieceType>(i)) & side_occ;
                while (piece_bb)
                {
                    int square = bits::pop_bit(piece_bb);
                    if (color == static_cast<I8>(Color::WHITE))
                    {
                        square ^= 0b111000;
                    }
                    score_opening += (material_score[OPENING][i] + positional_score[OPENING][i][square]) * color_offset[color];
                    score_endgame += (material_score[ENDGAME][i] + positional_score[ENDGAME][i][square]) * color_offset[color];
                }
            }
        }
    }

    int evaluate(Board &board)
    {
        int score = 0, score_opening = 0, score_endgame = 0, game_phase_score = 0;
        Color side = board.get_side_to_move();

        // get phase
        Phase phase = get_game_phase_score(board, game_phase_score);

        // get material score
        get_material_and_positional_score(board, score_opening, score_endgame);

        // get positional score
        // score += get_positional_score(board, phase);

        if (phase == MIDDLEGAME)
        {
            score = (score_opening * game_phase_score +
                     score_endgame * (opening_phase_score - game_phase_score)) /
                    opening_phase_score;
        }
        else if (phase == OPENING)
        {
            score = score_opening;
        }
        else
        {
            score = score_endgame;
        }

        score = score * (100 - board.get_fifty_move_counter()) / 100;

        return (score * color_offset[static_cast<I8>(side)]);
    }
}