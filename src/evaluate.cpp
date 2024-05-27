#include <iostream>
#include <cassert>

#include "evaluate.h"

#include "types.h"
#include "defs.h"
#include "board/board.h"
#include "utils/eval_terms.h"
#include "utils/bits.h"

using namespace elixir::bits;

namespace elixir::eval {
    EvalScore evaluate_pawns(const Board& board, const Color side) {
        Bitboard ours = board.color_occupancy(side);
        Bitboard pawns = board.pawns() & ours;
        EvalScore score = 0;
        while (pawns) {
            int sq_ = pop_bit(pawns);
            int file = get_file(sq(sq_));

            if (Files[file] & pawns) {
                score -= stacked_pawn_penalty;
            }            
        }

        return (side == Color::WHITE) ? score : -score;
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
        if (count_bits(ours & board.bishops()) >= 2) {
            score += bishop_pair_bonus;
        }
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
        EvalInfo e_info = EvalInfo(board.get_eval());

        e_info.add_score(evaluate_pawns(board, Color::WHITE));
        e_info.add_score(evaluate_pawns(board, Color::BLACK));

        e_info.add_score(evaluate_knights(board, Color::WHITE));
        e_info.add_score(evaluate_knights(board, Color::BLACK));

        e_info.add_score(evaluate_bishops(board, Color::WHITE));
        e_info.add_score(evaluate_bishops(board, Color::BLACK));

        e_info.add_score(evaluate_rooks(board, Color::WHITE));
        e_info.add_score(evaluate_rooks(board, Color::BLACK));

        e_info.add_score(evaluate_queens(board, Color::WHITE));
        e_info.add_score(evaluate_queens(board, Color::BLACK));

        score_opening = e_info.opening_score();
        score_endgame = e_info.endgame_score();
        int phase = count_bits(board.minors()) + 2 * count_bits(board.rooks()) + 4 * count_bits(board.queens());
        phase = std::clamp(phase, 0, 24);
        score = (score_opening * phase + score_endgame * (24 - phase)) / 24;
        return ((side == Color::WHITE) ? score : -score) + 10;
    }
}