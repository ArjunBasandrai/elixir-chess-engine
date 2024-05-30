#include "history.h"

namespace elixir {
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

    void History::update_history(Square from, Square to, int depth) {
        int ifrom = static_cast<int>(from);
        int ito = static_cast<int>(to);
        int &score = history[ifrom][ito];
        score += scale_bonus(score, depth * depth);
    }

    int History::get_history(Square from, Square to) const {
        return history[static_cast<int>(from)][static_cast<int>(to)];
    }
}