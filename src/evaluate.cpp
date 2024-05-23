#include <iostream>
#include <cassert>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/bits.h"

using namespace elixir::bits;

namespace elixir::eval {
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

    EvalScore evaluate_knights(const Board &board, const Color side) {
        Bitboard ours = board.color_occupancy(side);
        Bitboard knights = board.knights() & ours;
        EvalScore score = 0;
        while (knights) {
            int sq_ = pop_bit(knights);
            int mobility_count = count_bits(attacks::get_knight_attacks(sq(sq_)) & ~ours);
            score += knight_mobility[mobility_count];
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_bishops(const Board &board, const Color side) {
        Bitboard ours = board.color_occupancy(side);
        Bitboard bishops = board.bishops() & ours;
        EvalScore score = 0;
        while (bishops) {
            int sq_ = pop_bit(bishops);
            int mobility_count = count_bits(attacks::get_bishop_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += bishop_mobility[mobility_count];
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_rooks(const Board &board, const Color side) {
        Bitboard ours = board.color_occupancy(side);
        Bitboard rooks = board.rooks() & ours;
        EvalScore score = 0;
        while (rooks) {
            int sq_ = pop_bit(rooks);
            int mobility_count = count_bits(attacks::get_rook_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += rook_mobility[mobility_count];
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_queens(const Board &board, const Color side) {
        Bitboard ours = board.color_occupancy(side);
        Bitboard queens = board.queens() & ours;
        EvalScore score = 0;
        while (queens) {
            int sq_ = pop_bit(queens);
            int mobility_count = count_bits(attacks::get_queen_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += queen_mobility[mobility_count];
        }

        return (side == Color::WHITE) ? score : -score;
    }

    int evaluate(Board& board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();
        EvalScore eval = board.get_eval();

        eval += evaluate_knights(board, Color::WHITE);
        eval += evaluate_knights(board, Color::BLACK);

        eval += evaluate_bishops(board, Color::WHITE);
        eval += evaluate_bishops(board, Color::BLACK);

        eval += evaluate_rooks(board, Color::WHITE);
        eval += evaluate_rooks(board, Color::BLACK);

        eval += evaluate_queens(board, Color::WHITE);
        eval += evaluate_queens(board, Color::BLACK);

        score_opening = O(eval);
        score_endgame = E(eval);
        int phase = count_bits(board.minors()) + 2 * count_bits(board.rooks()) + 4 * count_bits(board.queens());
        phase = std::clamp(phase, 0, 24);
        score = (score_opening * phase + score_endgame * (24 - phase)) / 24;
        return (side == Color::WHITE) ? score : -score;
    }
}