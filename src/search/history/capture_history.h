#pragma once

#include "defs.h"
#include "move.h"

namespace elixir {
    class CaptureHistory {
      public:
        CaptureHistory() {
            history.resize(64);
            clear();
        }
        ~CaptureHistory() = default;

        void clear();

        void update_history(const Color side, const Square from, const Square to, const int depth);
        int get_capture_history(const Color side, const Square from, const Square to) const;

        void penalize(const Color side, const int depth, const MoveList& bad_captures);

      private:
        CaptureHistoryArray history;
    };
}