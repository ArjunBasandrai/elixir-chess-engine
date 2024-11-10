#pragma once

#include "defs.h"
#include "move.h"

namespace elixir {
    class QuietHistory {
      public:
        QuietHistory() {
            history.resize(12);
            clear();
        }
        ~QuietHistory() = default;

        void clear();

        void update_history(const Piece piece, const Square from, const Square to, const int depth, const MoveList &bad_quiets);
        int get_quiet_history(const Piece piece, const Square from, const Square to) const;

      private:
        QuietHistoryArray history;
    };
}