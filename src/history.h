#pragma once

#include "types.h"
#include "defs.h"

namespace elixir {
    class History {
    public:
        History() = default;
        ~History() = default;

        inline void update_history(Square from, Square to, int depth) {
            history[static_cast<int>(from)][static_cast<int>(to)] += depth * depth;
            int hist_max = 1024;
            history[static_cast<int>(from)][static_cast<int>(to)] = std::min(history[static_cast<int>(from)][static_cast<int>(to)], hist_max);
        }

        inline int get_history(Square from, Square to) const {
            return history[static_cast<int>(from)][static_cast<int>(to)];
        }

        inline void clear() {
            for (int i = 0; i < 64; i++) {
                for (int j = 0; j < 64; j++) {
                    history[i][j] = 0;
                }
            }
        }

    private:
        int history[64][64] = {0};
    };
}