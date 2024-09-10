#pragma once

#include <algorithm>

namespace elixir {
    extern int HISTORY_GRAVITY;

    inline int history_bonus(int depth) {
        return std::min(300 * depth - 150, 4000);
    }

    inline int history_malus(int depth) {
        return -std::min(600 * depth - 300, 8000);
    }

    inline int scale_bonus(int score, int bonus) {
        return bonus - score * std::abs(bonus) / HISTORY_GRAVITY;
    }
}