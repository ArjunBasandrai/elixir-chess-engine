#pragma once

#include "../ss.h"
#include "move.h"
#include "types.h"

namespace elixir {
    class ContiuationHistory {
      public:
        ContiuationHistory() {
            cont_hist.resize(12);
            clear();
        }
        ~ContiuationHistory() = default;

        void clear();

        ContHistEntry *get_cont_hist_entry(move::Move &move) {
            return &cont_hist[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())];
        }

        void update_chs(move::Move &move, search::SearchStack *ss, MoveList &bad_quiets, int depth);
        int get_chs(move::Move &move, const search::SearchStack *ss) const;

      private:
        ContHistArray cont_hist;

        void update_single_chs(move::Move &move, search::SearchStack *ss, int depth,
                               bool is_bad_quiet);
    };
}