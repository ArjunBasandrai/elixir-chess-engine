#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"
#include "ss.h"

namespace elixir {
    extern int HISTORY_GRAVITY;

    inline int history_bonus(int depth) {
        return std::min(300 * depth - 150, 4000);
    }

    inline int history_malus(int depth) {
        return -std::min(600 * depth - 300, 8000);
    }

    inline int scale_bonus(int score, int bonus) {
        return bonus - score * std::abs(bonus) / HISTORY_GRAVITY;
    }

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

    class ContiuationHistory {
        public:
            ContiuationHistory() {
                cont_hist.resize(12);
                for (auto &to_sq: cont_hist) {
                    to_sq.resize(64);
                    for (auto &entry: to_sq) {
                        entry.resize(12);
                        for (auto &from_sq: entry) {
                            from_sq.resize(64);
                        }
                    }
                }
                clear();
            }
            ~ContiuationHistory() = default;

            void clear();

            ContHistEntry *get_cont_hist_entry(move::Move& move) {
                return &cont_hist[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())];
            }

            void update_chs(move::Move& move, search::SearchStack *ss, MoveList &bad_quiets, int depth);
            int get_chs(move::Move& move, const search::SearchStack *ss) const;
            
        private:
            ContHistArray cont_hist;

            void update_single_chs(move::Move& move, search::SearchStack *ss, int depth, bool is_bad_quiet);
    };

    class CounterMoveHistory {
        public:
            CounterMoveHistory() {
                clear();
            }
            ~CounterMoveHistory() = default;

            void clear();

            void update_countermove(Color side, Square from, Square to, move::Move countermove);
            move::Move get_countermove(Color side, Square from, Square to) const;
        
        private:
            move::Move counter_moves[2][64][64] = {move::NO_MOVE};
    };

    class History {
      public:
        History()  {
            clear();
        }
        ~History() = default;

        void clear();

        int get_history(move::Move& move, const search::SearchStack *ss) const {
          return 2 * quiet_history.get_quiet_history(move.get_from(), move.get_to()) + (continuation_history.get_chs(move, ss - 1) + continuation_history.get_chs(move, ss - 2));
        }

        QuietHistory quiet_history;
        ContiuationHistory continuation_history;
        CounterMoveHistory countermove_history;
    };
}