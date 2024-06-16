#include "history.h"
#include "defs.h"
#include "move.h"
#include "types.h"

namespace elixir {
    int HISTORY_GRAVITY = 8289;

    void History::clear() {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                history[i][j] = 0;
            }
        }
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                counter_moves[0][i][j] = move::NO_MOVE;
                counter_moves[1][i][j] = move::NO_MOVE;
            }
        }
    }

    int History::scale_bonus(int score, int bonus) {
        return bonus - score * std::abs(bonus) / HISTORY_GRAVITY;
    }

    void History::update_history(Square from, Square to, int depth, MoveList &bad_quiets) {
        int ifrom  = static_cast<int>(from);
        int ito    = static_cast<int>(to);
        int &score = history[ifrom][ito];
        score += scale_bonus(score, depth * depth);

        const int penalty = -depth * depth;
        for (const auto &move : bad_quiets) {
            const int bfrom = static_cast<int>(move.get_from());
            const int bto   = static_cast<int>(move.get_to());

            int &bad_quiet_score = history[bfrom][bto];
            bad_quiet_score += scale_bonus(bad_quiet_score, penalty);
        }
    }

    int History::get_history(Square from, Square to) const {
        return history[static_cast<int>(from)][static_cast<int>(to)];
    }

    void History::update_countermove(Color side, Square from, Square to, move::Move countermove) {
        counter_moves[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)] =
            countermove;
    }

    move::Move History::get_countermove(Color side, Square from, Square to) const {
        return counter_moves[static_cast<int>(side)][static_cast<int>(from)][static_cast<int>(to)];
    }

}