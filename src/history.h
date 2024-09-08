#pragma once

#include <vector>

#include "defs.h"
#include "move.h"
#include "types.h"
#include "ss.h"

namespace elixir {
    extern int HISTORY_GRAVITY;
    class History {
      public:
        History()  {
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

            capthist.resize(12);
            for (auto &to_sq: capthist) {
                to_sq.resize(64);
                for (auto &entry: to_sq) {
                    entry.resize(12);
                }
            }
            
            clear();
        }
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
          return 2 * get_quiet_history(move.get_from(), move.get_to()) + (get_chs(move, ss - 1) + get_chs(move, ss - 2));
        }

        int get_capthist(move::Move move, Piece captured) const {
            return capthist[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())][static_cast<int>(captured)];
        }

        void update_single_capthist(move::Move& move, Piece captured, int depth, bool is_bad_noisy) {
            int bonus = (is_bad_noisy) ? history_malus(depth) : history_bonus(depth);
            int &score = capthist[static_cast<int>(move.get_piece())][static_cast<int>(move.get_to())][static_cast<int>(captured)];
            score += scale_bonus(score, bonus);
        }
        void update_capthist(move::Move &move, Piece captured, MovePieceList bad_noisies, int depth);

        ContHistArray cont_hist;

      private:
        int history_bonus(int depth) {
            return std::min(300 * depth - 150, 4000);
        }

        int history_malus(int depth) {
            return -std::min(600 * depth - 300, 8000);
        }
        int scale_bonus(int score, int bonus);
        int history[64][64]                 = {0};
        move::Move counter_moves[2][64][64] = {move::NO_MOVE};
        std::vector<std::vector<std::vector<int>>> capthist;

        void update_single_chs(move::Move& move, search::SearchStack *ss, int depth, bool is_bad_quiet);
    };
}