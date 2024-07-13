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

    void Evaluator::init_evaluator(const Board &board) {
        const Square white_king_square = board.get_king_square(Color::WHITE);
        const Square black_king_square = board.get_king_square(Color::BLACK);

        const Bitboard white_king_attacks = attacks::get_king_attacks(white_king_square);
        const Bitboard black_king_attacks = attacks::get_king_attacks(black_king_square);

        e_info.king_zones[0] = white_king_attacks | (white_king_attacks << 8);
        e_info.king_zones[1] = black_king_attacks | (black_king_attacks << 8);
    }

    void Evaluator::evaluate_pawns(const Board &board, const Color side) {
        const Bitboard ours        = board.color_occupancy(side);
        const Bitboard theirs      = board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        Bitboard pawns             = board.pawns() & ours;
        const Bitboard our_pawns   = pawns;
        EvalScore score            = 0;
        const I8 icolor            = static_cast<I8>(side);
        const Color them           = static_cast<Color>(icolor ^ 1);
        const Square our_king_sq   = board.get_king_square(side);
        const Square their_king_sq = board.get_king_square(them);
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

                int our_king_distance   = get_square_distance(sq(sq_), our_king_sq);
                int their_king_distance = get_square_distance(sq(sq_), their_king_sq);

                score += our_king_pp_proximity[our_king_distance];
                score += their_king_pp_proximity[their_king_distance];

                if (count_bits(board.majors() & theirs) == 0) {
                    score += passed_pawn_no_threat;
                    TRACE_INCREMENT(passed_pawn_no_threat, icolor);
                }

                TRACE_INCREMENT(our_king_pp_proximity[our_king_distance], icolor);
                TRACE_INCREMENT(their_king_pp_proximity[their_king_distance], icolor);
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

            if (attacks::get_pawn_attacks(side, sq(sq_)) & board.minors() & theirs) {
                score += pawn_minor_threat;
                TRACE_INCREMENT(pawn_minor_threat, icolor);
            }

            if (attacks::get_pawn_attacks(side, sq(sq_)) & board.majors() & theirs) {
                score += pawn_major_threat;
                TRACE_INCREMENT(pawn_major_threat, icolor);
            }
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    void Evaluator::evaluate_knights(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        const Bitboard theirs =
            board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard knights           = board.knights() & ours;
        EvalScore score            = 0;
        I8 icolor                  = static_cast<I8>(side);
        I8 them                    = icolor ^ 1;
        while (knights) {
            const int sq_            = pop_bit(knights);
            const int file           = get_file(sq(sq_));
            const Bitboard attacks   = attacks::get_knight_attacks(sq(sq_));
            const int mobility_count = count_bits(attacks & ~ours);
            score += knight_mobility[mobility_count];
            TRACE_INCREMENT(knight_mobility[mobility_count], static_cast<I8>(side));

            if (! (masks::passed_pawn_masks[icolor][sq_] & masks::isolated_pawn_masks[file] &
                   their_pawns)) {
                const int colored_sq = (icolor == 0) ? sq_ ^ 56 : sq_;
                score += knight_outpost_bonus[colored_sq];
                TRACE_INCREMENT(knight_outpost_bonus[colored_sq], icolor);
            }

            const Bitboard in_king_zone = attacks & e_info.king_zones[them];
            if (in_king_zone) {
                const int idx = std::min(7, count_bits(in_king_zone));
                score += knight_king_proximity_bonus[idx];
                TRACE_INCREMENT(knight_king_proximity_bonus[idx], icolor);
            }
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    void Evaluator::evaluate_bishops(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        Bitboard bishops    = board.bishops() & ours;
        EvalScore score     = 0;
        I8 icolor           = static_cast<I8>(side);
        I8 them             = icolor ^ 1;
        if (count_bits(ours & board.bishops()) >= 2) {
            score += bishop_pair_bonus;
            TRACE_INCREMENT(bishop_pair_bonus, static_cast<I8>(side));
        }
        while (bishops) {
            int sq_                = pop_bit(bishops);
            const Bitboard attacks = attacks::get_bishop_attacks(sq(sq_), board.occupancy());
            int mobility_count     = count_bits(attacks & ~ours);
            score += bishop_mobility[mobility_count];
            TRACE_INCREMENT(bishop_mobility[mobility_count], static_cast<I8>(side));

            const Bitboard in_king_zone = attacks & e_info.king_zones[them];
            if (in_king_zone) {
                const int idx = std::min(7, count_bits(in_king_zone));
                score += bishop_king_proximity_bonus[idx];
                TRACE_INCREMENT(bishop_king_proximity_bonus[idx], icolor);
            }
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    void Evaluator::evaluate_rooks(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        const Bitboard theirs =
            board.color_occupancy(static_cast<Color>(static_cast<I8>(side) ^ 1));
        const Bitboard our_pawns   = board.pawns() & ours;
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard rooks             = board.rooks() & ours;
        EvalScore score            = 0;
        I8 icolor                  = static_cast<I8>(side);
        I8 them                    = icolor ^ 1;
        while (rooks) {
            int sq_                = pop_bit(rooks);
            int file               = get_file(sq(sq_));
            const Bitboard attacks = attacks::get_rook_attacks(sq(sq_), board.occupancy());
            int mobility_count     = count_bits(attacks & ~ours);
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

            const Bitboard in_king_zone = attacks & e_info.king_zones[them];
            if (in_king_zone) {
                const int idx = std::min(7, count_bits(in_king_zone));
                score += rook_king_proximity_bonus[idx];
                TRACE_INCREMENT(rook_king_proximity_bonus[idx], icolor);
            }
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    void Evaluator::evaluate_queens(const Board &board, const Color side) {
        const Bitboard ours = board.color_occupancy(side);
        Bitboard queens     = board.queens() & ours;
        EvalScore score     = 0;
        I8 icolor           = static_cast<I8>(side);
        I8 them             = icolor ^ 1;
        while (queens) {
            int sq_                = pop_bit(queens);
            const Bitboard attacks = attacks::get_queen_attacks(sq(sq_), board.occupancy());
            int mobility_count     = count_bits(attacks & ~ours);
            score += queen_mobility[mobility_count];
            TRACE_INCREMENT(queen_mobility[mobility_count], static_cast<I8>(side));

            const Bitboard in_king_zone = attacks & e_info.king_zones[them];
            if (in_king_zone) {
                const int idx = std::min(7, count_bits(in_king_zone));
                score += queen_king_proximity_bonus[idx];
                TRACE_INCREMENT(queen_king_proximity_bonus[idx], icolor);
            }
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    void Evaluator::evaluate_kings(const Board &board, const Color side) {
        const Color them           = static_cast<Color>(static_cast<I8>(side) ^ 1);
        const Bitboard ours        = board.color_occupancy(side);
        const Bitboard theirs      = board.color_occupancy(them);
        const Bitboard our_pawns   = board.pawns() & ours;
        const Bitboard their_pawns = board.pawns() & theirs;
        Bitboard kings             = board.king() & ours;
        const auto sq_             = board.get_king_square(side);
        const int file             = get_file(sq_);
        const int rank             = get_rank(sq_);
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

        Square their_king_square = board.get_king_square(them);
        const int king_file      = get_file(sq_);
        const int king_rank      = get_rank(sq_);
        for (int safety_file = std::max<int>(FILE_A, king_file - 1);
             safety_file <= std::min<int>(FILE_H, king_file + 1); safety_file++) {
            const int file_index = (king_file == safety_file)                           ? 8
                                   : (king_file >= FILE_E) == (king_file < safety_file) ? 0
                                                                                        : 16;

            Bitboard pawn_mask = our_pawns & Files[safety_file];
            int distance =
                pawn_mask ? std::abs(get_rank(relative_last_sq(pawn_mask, side)) - king_rank) : 7;

            score += pawn_storm_table[file_index + distance];
            TRACE_INCREMENT(pawn_storm_table[file_index + distance], static_cast<I8>(side));

            pawn_mask = their_pawns & Files[safety_file];
            distance =
                pawn_mask ? std::abs(get_rank(relative_last_sq(pawn_mask, side)) - king_rank) : 7;

            score += pawn_shelter_table[file_index + distance];
            TRACE_INCREMENT(pawn_shelter_table[file_index + distance], static_cast<I8>(side));
        }

        e_info.add_score((side == Color::WHITE) ? score : -score);
    }

    int Evaluator::evaluate(const Board &board) {
        Score score = 0, score_opening = 0, score_endgame = 0;
        Color side = board.get_side_to_move();

        e_info = EvalInfo(board.get_eval());

        init_evaluator(board);

        evaluate_pawns(board, Color::WHITE);
        evaluate_pawns(board, Color::BLACK);

        evaluate_knights(board, Color::WHITE);
        evaluate_knights(board, Color::BLACK);

        evaluate_bishops(board, Color::WHITE);
        evaluate_bishops(board, Color::BLACK);

        evaluate_rooks(board, Color::WHITE);
        evaluate_rooks(board, Color::BLACK);

        evaluate_queens(board, Color::WHITE);
        evaluate_queens(board, Color::BLACK);

        evaluate_kings(board, Color::WHITE);
        evaluate_kings(board, Color::BLACK);

        score_opening = e_info.opening_score();
        score_endgame = e_info.endgame_score();
        int phase     = board.get_phase();
        score         = (score_opening * phase + score_endgame * (24 - phase)) / 24;
        return ((side == Color::WHITE) ? score : -score) + TEMPO;
    }

    int evaluate(const Board &board) {
        return evaluator.evaluate(board);
    }
}