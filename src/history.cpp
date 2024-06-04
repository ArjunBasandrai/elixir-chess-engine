#include "types.h"
#include "defs.h"
#include "history.h"
#include "move.h"

namespace elixir {
    int HISTORY_GRAVITY = 8195;

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
}