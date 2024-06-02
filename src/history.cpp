#include "types.h"
#include "defs.h"
#include "history.h"
#include "move.h"
#include "board/board.h"

namespace elixir {
    class Board;

    void History::clear() {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                history[i][j] = 0;
            }
        }
    }

    int History::scale_bonus(int score, int bonus) {
        return bonus - score * std::abs(bonus) / HISTORY_GRAVITY;
    }

    void History::update_history(Square from, Square to, int depth, MoveList &bad_quiets) {
        int ifrom = static_cast<int>(from);
        int ito = static_cast<int>(to);
        int &score = history[ifrom][ito];
        score += scale_bonus(score, depth * depth);

        const int penalty = -depth * depth;
        for (const auto &move : bad_quiets) {
            const int bfrom = static_cast<int>(move.get_from());
            const int bto = static_cast<int>(move.get_to());

            int &bad_quiet_score = history[bfrom][bto];
            bad_quiet_score += scale_bonus(bad_quiet_score, penalty);
        }
    }

    int History::get_history(Square from, Square to) const {
        return history[static_cast<int>(from)][static_cast<int>(to)];
    }

    void History::update_cont_history(const Board& board, int color, int piece_type, int to, int depth, MoveList &bad_quiets, search::SearchStack *ss) {
        const auto update_entry = [this, color, &ss](
            int piece_type, int to, int plies, int bonus
        ) {
            if ((ss - plies)->move != move::NO_MOVE) {
                int &score = (ss - plies)->cont_hist[color][piece_type][to];
                score += scale_bonus(score, bonus);
            }
        };

        int bonus = depth * depth;

        update_entry(piece_type, to, 1, bonus);
        update_entry(piece_type, to, 2, bonus);

        const int penalty = -depth * depth;
        for (const auto &bmove: bad_quiets) {
            const int bto = static_cast<int>(bmove.get_to());
            const int bpiece_type = static_cast<int>(board.piece_to_piecetype(board.piece_on(bmove.get_from())));
            update_entry(bpiece_type, bto, 1, penalty);
            update_entry(bpiece_type, bto, 2, penalty);
        }
    }

    int History::get_cont_history(int color, int piece_type, int to, const search::SearchStack *ss) const {
        int score = 0;
        if ((ss - 1)->move != move::NO_MOVE) {
            score += (ss - 1)->cont_hist[color][piece_type][to];
        }
        if ((ss - 2)->move != move::NO_MOVE) {
            score += (ss - 2)->cont_hist[color][piece_type][to];
        }
        return score;
    }
}