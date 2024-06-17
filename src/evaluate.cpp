#include <cassert>
#include <iostream>

#include "evaluate.h"

#include "board/board.h"
#include "defs.h"
#include "texel/texel.h"
#include "types.h"
#include "utils/bits.h"
#include "utils/eval_terms.h"
#include "utils/masks.h"

using namespace elixir::bits;
using namespace elixir::texel;

namespace elixir::eval {
    int TEMPO           = 7;
    int MP_PAWN         = 86;
    int MP_KNIGHT       = 304;
    int MP_BISHOP       = 360;
    int MP_ROOK         = 466;
    int MP_QUEEN        = 905;
    int MP_KING         = 20903;
    int piece_values[7] = {MP_PAWN, MP_KNIGHT, MP_BISHOP, MP_ROOK, MP_QUEEN, MP_KING, 0};

    EvalScore evaluate_pawns(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        Bitboard pawns      = board.pawns() & ours;
        Bitboard our_pawns  = pawns;
        EvalScore score     = 0;
        I8 icolor           = static_cast<I8>(side);
        while (pawns) {
            const int sq_           = pop_bit(pawns);
            const int file          = get_file(sq(sq_));
            const int rank          = get_rank(sq(sq_));
            const int relative_rank = (side == Color::WHITE) ? 7 - rank : rank;

            if (Files[file] & pawns) {
                score -= stacked_pawn_penalty;
                TRACE_DECREMENT(stacked_pawn_penalty, icolor);
            }

            if (! (masks::passed_pawn_masks[icolor][sq_] & board.pawns())) {
                score += passed_pawn_bonus[relative_rank];
                TRACE_INCREMENT(passed_pawn_bonus[relative_rank], icolor);
            }

            if (! (masks::isolated_pawn_masks[file] & our_pawns)) {
                score -= isolated_pawn_penalty[file];
                TRACE_DECREMENT(isolated_pawn_penalty[file], icolor);
            }

            if (attacks::get_pawn_attacks(static_cast<Color>(icolor ^ 1), sq(sq_)) & our_pawns) {
                score += supported_pawn_bonus[relative_rank];
                TRACE_INCREMENT(supported_pawn_bonus[relative_rank], icolor);
            }

            if (masks::isolated_pawn_masks[file] & Ranks[rank] & our_pawns) {
                score += pawn_duo_bonus[relative_rank];
                TRACE_INCREMENT(pawn_duo_bonus[relative_rank], icolor);
            }
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_knights(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        const Bitboard theirs =
            board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard knights           = board.knights() & ours;
        EvalScore score            = 0;
        I8 icolor                  = static_cast<I8>(side);
        while (knights) {
            const int sq_            = pop_bit(knights);
            const int file           = get_file(sq(sq_));
            const int mobility_count = count_bits(attacks::get_knight_attacks(sq(sq_)) & ~ours);
            score += knight_mobility[mobility_count];
            TRACE_INCREMENT(knight_mobility[mobility_count], static_cast<I8>(side));

            if (! (masks::passed_pawn_masks[icolor][sq_] & masks::isolated_pawn_masks[file] &
                   their_pawns)) {
                const int colored_sq = (icolor == 0) ? sq_ ^ 56 : sq_;
                score += knight_outpost_bonus[colored_sq];
                TRACE_INCREMENT(knight_outpost_bonus[colored_sq], icolor);
            }
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_bishops(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        Bitboard bishops    = board.bishops() & ours;
        EvalScore score     = 0;
        if (count_bits(ours & board.bishops()) >= 2) {
            score += bishop_pair_bonus;
            TRACE_INCREMENT(bishop_pair_bonus, static_cast<I8>(side));
        }
        while (bishops) {
            int sq_ = pop_bit(bishops);
            int mobility_count =
                count_bits(attacks::get_bishop_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += bishop_mobility[mobility_count];
            TRACE_INCREMENT(bishop_mobility[mobility_count], static_cast<I8>(side));
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_rooks(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        const Bitboard theirs =
            board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        const Bitboard our_pawns   = board.pawns() & ours;
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard rooks             = board.rooks() & ours;
        EvalScore score            = 0;
        while (rooks) {
            int sq_  = pop_bit(rooks);
            int file = get_file(sq(sq_));
            int mobility_count =
                count_bits(attacks::get_rook_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += rook_mobility[mobility_count];
            TRACE_INCREMENT(rook_mobility[mobility_count], static_cast<I8>(side));

            if (! (Files[file] & our_pawns)) {
                if (! (Files[file] & their_pawns)) {
                    score += rook_open_file_bonus[file];
                    TRACE_INCREMENT(rook_open_file_bonus[file], static_cast<I8>(side));
                } else {
                    score += rook_semi_open_file_bonus[file];
                    TRACE_INCREMENT(rook_semi_open_file_bonus[file], static_cast<I8>(side));
                }
            }
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_queens(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        Bitboard queens     = board.queens() & ours;
        EvalScore score     = 0;
        while (queens) {
            int sq_ = pop_bit(queens);
            int mobility_count =
                count_bits(attacks::get_queen_attacks(sq(sq_), board.occupancy()) & ~ours);
            score += queen_mobility[mobility_count];
            TRACE_INCREMENT(queen_mobility[mobility_count], static_cast<I8>(side));
        }

        return (side == Color::WHITE) ? score : -score;
    }

    EvalScore evaluate_kings(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        const Bitboard theirs =
            board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        const Bitboard our_pawns   = board.pawns() & ours;
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard kings             = board.king() & ours;
        const auto sq_             = board.get_king_square(side);
        const int file             = get_file(sq_);
        EvalScore score            = 0;

        if (! (Files[file] & our_pawns)) {
            if (! (Files[file] & their_pawns)) {
                score -= king_open_file_penalty[file];
                TRACE_DECREMENT(king_open_file_penalty[file], static_cast<I8>(side));
            } else {
                score -= king_semi_open_file_penalty[file];
                TRACE_DECREMENT(king_semi_open_file_penalty[file], static_cast<I8>(side));
            }
        }

        return (side == Color::WHITE) ? score : -score;
    }


    int evaluate(Board &board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side      = board.get_side_to_move();
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

        e_info.add_score(evaluate_kings(board, Color::WHITE));
        e_info.add_score(evaluate_kings(board, Color::BLACK));

        score_opening = e_info.opening_score();
        score_endgame = e_info.endgame_score();
        int phase     = board.get_phase();
        score         = (score_opening * phase + score_endgame * (24 - phase)) / 24;
        return ((side == Color::WHITE) ? score : -score) + TEMPO;
    }
}