#pragma once

#include <array>
#include <vector>

#include "move.h"
#include "types.h"

namespace elixir {
    const int pawn_correction_history_size = 16384;
    const int non_pawn_correction_history_size = 32768;
    const int pawn_history_size = 512;
    const int correction_history_limit = 1024;

    using PawnCorrectionHistoryTable = std::vector<std::array<int, pawn_correction_history_size>>;
    using NonPawnCorrectionHistoryTable = std::vector<std::array<std::array<int, non_pawn_correction_history_size>,2>>;

    class CorrectionHistory {
      public:
        CorrectionHistory() {
            pawn_corr_hist.resize(2);
            non_pawn_corr_hist.resize(2);
            clear();
        }
        ~CorrectionHistory() = default;

        void clear();

        int correct_static_eval(int static_eval, Color side, U64 pawn_hash_key, const std::array<U64, 2>& non_pawn_hash_keys) const;
        void update_correction_history(int bonus, Color side, U64 pawn_hash_key, const std::array<U64, 2>& non_pawn_hash_keys);
      private:
        PawnCorrectionHistoryTable pawn_corr_hist;
        NonPawnCorrectionHistoryTable non_pawn_corr_hist;
    };
}
