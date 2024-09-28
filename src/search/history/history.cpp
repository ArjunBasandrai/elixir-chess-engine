#include <iostream>

#include "../ss.h"
#include "defs.h"
#include "history.h"
#include "move.h"
#include "types.h"

namespace elixir {
    int HISTORY_GRAVITY          = 8916;
    int HISTORY_BONUS_MULTIPLIER = 266;
    int HISTORY_MALUS_MULTIPLIER = 637;
    int HISTORY_BONUS_SUBTRAHEND = 154;
    int HISTORY_MALUS_SUBTRAHEND = 314;
    int HISTORY_BONUS_MAX        = 3930;
    int HISTORY_MALUS_MAX        = 7849;

    void History::clear() {
        quiet_history.clear();
        continuation_history.clear();
        countermove_history.clear();
    }

    void QuietHistory::clear() {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                history[i][j] = 0;
            }
        }
    }

    void QuietHistory::update_history(Square from, Square to, int depth, MoveList &bad_quiets) {
        int ifrom  = static_cast<int>(from);
        int ito    = static_cast<int>(to);
        int &score = history[ifrom][ito];
        int bonus  = scale_bonus(score, history_bonus(depth));
        score += bonus;

        const int penalty = history_malus(depth);
        for (const auto &move : bad_quiets) {
            const int bfrom = static_cast<int>(move.get_from());
            const int bto   = static_cast<int>(move.get_to());

            int &bad_quiet_score = history[bfrom][bto];
            bad_quiet_score += scale_bonus(bad_quiet_score, penalty);
        }
    }

    int QuietHistory::get_quiet_history(Square from, Square to) const {
        return history[static_cast<int>(from)][static_cast<int>(to)];
    }

    void CounterMoveHistory::clear() {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                counter_moves[0][i][j] = move::NO_MOVE;
                counter_moves[1][i][j] = move::NO_MOVE;
            }
        }
    }

    void CounterMoveHistory::update_countermove(Color side, Square from, Square to,
                                                move::Move countermove) {
        counter_moves[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)] =
            countermove;
    }

    move::Move CounterMoveHistory::get_countermove(Color side, Square from, Square to) const {
        return counter_moves[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)];
    }

    void ContiuationHistory::clear() {
        for (auto &to_sq : cont_hist) {
            for (auto &entry : to_sq) {
                for (auto &from_sq : entry) {
                    for (auto &piece : from_sq) {
                        piece = 0;
                    }
                }
            }
        }
    }

    void ContiuationHistory::update_chs(move::Move &move, search::SearchStack *ss,
                                        MoveList &bad_quiets, int depth) {
        update_single_chs(move, ss - 1, depth, false);
        update_single_chs(move, ss - 2, depth, false);
        update_single_chs(move, ss - 4, depth, false);

        for (auto &bad_quiet : bad_quiets) {
            update_single_chs(bad_quiet, ss - 1, depth, true);
            update_single_chs(bad_quiet, ss - 2, depth, true);
            update_single_chs(bad_quiet, ss - 4, depth, true);
        }
    }

    int ContiuationHistory::get_chs(move::Move &move, const search::SearchStack *ss) const {
        if (! ss->move || ss->cont_hist == nullptr)
            return 0;
        return (
            *(ss)->cont_hist)[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())];
    }

    void ContiuationHistory::update_single_chs(move::Move &move, search::SearchStack *ss, int depth,
                                               bool is_bad_quiet) {
        if (! ss->move || ss->cont_hist == nullptr)
            return;
        int &score =
            (*(ss)->cont_hist)[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())];
        int bonus = (is_bad_quiet) ? history_malus(depth) : history_bonus(depth);
        score += scale_bonus(score, bonus);
    }

    void CorrectionHistory::clear() {
        for (auto &entry : corr_hist) {
            for (auto &piece : entry) {
                piece = 0;
            }
        }
    }

    int CorrectionHistory::correct_static_eval(int static_eval, Color side, U64 pawn_hash_key) const {
        const int correction_score = corr_hist[static_cast<int>(side)][pawn_hash_key & (pawn_correction_history_size - 1)];
        const int adjusted_score = static_eval + (correction_score * std::abs(correction_score)) / correction_history_limit;
        return std::clamp(adjusted_score, -MATE_FOUND + 1, MATE_FOUND - 1);
    }

    void CorrectionHistory::update_correction_history(int bonus, Color side, U64 pawn_hash_key) {
        int &score = corr_hist[static_cast<int>(side)][pawn_hash_key & (pawn_correction_history_size - 1)];
        score += scale_bonus(score, bonus, pawn_history_size);
    }
}