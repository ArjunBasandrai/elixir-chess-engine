#pragma once

#include "defs.h"
#include "move.h"

namespace elixir {
    class QuietHistory {
        public:
            QuietHistory() {
                clear();
            }
            ~QuietHistory() = default;
    
            void clear();

            void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
            int get_quiet_history(Square from, Square to) const;
    
        private:
            int history[64][64] = {0};
    };
}