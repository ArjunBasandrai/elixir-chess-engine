#pragma once

#include <memory>

#include "types.h"
#include "defs.h"
#include "move.h"
#include "search_stack.h"

namespace elixir {

    class Board;
    class History {
    public:
        History() {
            cont_history = std::make_unique<cont_hist_table>();
        }
        ~History() = default;

        void clear();
        void update_history(Square from, Square to, int depth, MoveList &bad_quiets);
        int get_history(Square from, Square to) const;

        void update_cont_history(const Board& board, int color, int piece_type, int to, int depth, MoveList &bad_quiets, search::SearchStack *ss);
        int get_cont_history(int color, int piece_type, int to, const search::SearchStack *ss) const;

        cont_hist_entry *get_conthist_entry(int color, int piece_type, int to) {
            return &(*cont_history)[color][piece_type][to];
        }

    private:
        int scale_bonus(int score, int bonus);
        const int HISTORY_GRAVITY = 8192;
        int history[64][64] = {0};
        std::unique_ptr<cont_hist_table> cont_history;
    };
}