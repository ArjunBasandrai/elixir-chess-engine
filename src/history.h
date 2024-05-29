#pragma once

#include "types.h"
#include "defs.h"

namespace elixir {
    class History {
    public:
        History() = default;
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth);
        int get_history(Square from, Square to) const;

    private:
        int scale_bonus(int score, int bonus);
        const int HISTORY_GRAVITY = 8192;
        int history[64][64] = {0};
    };
}