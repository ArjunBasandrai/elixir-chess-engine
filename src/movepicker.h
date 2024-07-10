#pragma once

#include "move.h"
#include "movegen.h"
#include "search.h"
#include "ss.h"
#include "utils/static_vector.h"

namespace elixir {
    enum class STAGE : U8 {
        HASH_MOVE,
        GEN_NOISY,
        NOISY,
        KILLER_1,
        KILLER_2,
        GEN_QUIET,
        QUIET,
        GEN_BAD_CAPTURES,
        BAD_CAPTURES,
        GEN_BAD_PROMOTION,
        BAD_PROMOTION,
        END
    };
    extern int MP_SEE;
    class MovePicker {
      public:
        MovePicker()  = default;
        ~MovePicker() = default;
        void init_mp(const Board &board, move::Move tt_move, search::SearchStack *ss, bool for_qs);
        move::Move next_move();
        move::Move first_move() {
            if (moves.size() == 0)
                return move::NO_MOVE;
            return moves[0];
        }

        void setup_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool skip_quiets);
        move::Move next(Board& board, search::SearchStack *ss);

      private:
        MoveList noisy_moves;
        StaticVector<int, 256> noisy_scores;
        MoveList quiet_moves;
        StaticVector<int, 256> quiet_scores;

        MoveList bad_captures;
        MoveList bad_promotions;

        STAGE stage;
        bool no_quiets;
        int noisy_size;
        int quiet_size;
        move::Move hash_move;
        move::Move killers[2];

        MoveList moves;
        StaticVector<int, 256> scores;
        void score_moves(const Board &board, const move::Move &tt_move,
                         const search::SearchStack *ss);
    };
}