#pragma once

#include <algorithm>

namespace elixir {
    extern int HISTORY_GRAVITY;

    extern int HISTORY_BONUS_MULTIPLIER;
    extern int HISTORY_MALUS_MULTIPLIER;
    extern int HISTORY_BONUS_SUBTRAHEND;
    extern int HISTORY_MALUS_SUBTRAHEND;
    extern int HISTORY_BONUS_MAX;
    extern int HISTORY_MALUS_MAX;

    inline int history_bonus(int depth) {
        return std::min(HISTORY_BONUS_MULTIPLIER * depth - HISTORY_BONUS_SUBTRAHEND,
                        HISTORY_BONUS_MAX);
    }

    inline int history_malus(int depth) {
        return -std::min(HISTORY_MALUS_MULTIPLIER * depth - HISTORY_MALUS_SUBTRAHEND,
                         HISTORY_MALUS_MAX);
    }

    inline int scale_bonus(int score, int bonus) {
        return bonus - score * std::abs(bonus) / HISTORY_GRAVITY;
    }
}