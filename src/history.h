#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"
#include "ss.h"

namespace elixir {
    extern int HISTORY_GRAVITY;
    class History {
      public:
        History()  = default;
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
        int get_quiet_history(Square from, Square to) const;

        void update_countermove(Color side, Square from, Square to, move::Move countermove);
        move::Move get_countermove(Color side, Square from, Square to) const;

        ContHistEntry *get_cont_hist_entry(move::Move& move) {
            return &cont_hist[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())];
        }

        void update_chs(move::Move& move, search::SearchStack *ss, MoveList &bad_quiets, int depth);
        int get_chs(move::Move& move, const search::SearchStack *ss) const;

        int get_history(move::Move& move, const search::SearchStack *ss) const {
          return get_quiet_history(move.get_from(), move.get_to()) +
                 2 * (get_chs(move, ss - 1) + get_chs(move, ss - 2));
        }

        ContHistArray cont_hist = {0};

      private:
        int history_bonus(int depth) {
            return std::min(300 * depth - 150, 4000);
        }
        int scale_bonus(int score, int bonus);
        int history[64][64]                 = {0};
        move::Move counter_moves[2][64][64] = {move::NO_MOVE};

        void update_single_chs(move::Move& move, search::SearchStack *ss, int depth, bool is_bad_quiet);
    };
}