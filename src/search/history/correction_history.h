#pragma once

#include <array>
#include <vector>

#include "move.h"
#include "types.h"

namespace elixir {
    const int pawn_correction_history_size = 16384;
    const int pawn_history_size            = 512;
    const int correction_history_limit     = 1024;

    using CorrectionHistoryTable = std::vector<std::array<int, pawn_correction_history_size>>;

    class CorrectionHistory {
      public:
        CorrectionHistory() {
            corr_hist.resize(2);
            clear();
        }
        ~CorrectionHistory() = default;

        void clear();

        int correct_static_eval(int static_eval, Color side, U64 pawn_hash_key) const;
        void update_correction_history(int bonus, Color side, U64 pawn_hash_key);

      private:
        CorrectionHistoryTable corr_hist;
    };
}
