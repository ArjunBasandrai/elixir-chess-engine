#pragma once

#include "utils/static_vector.h"
#include "move.h"
#include "movegen.h"
#include "search.h"

namespace elixir {
    enum class STAGE: U8 {
        HASH_MOVE,
        GEN_NOISY,
        NOISY,
        KILLER_1,
        KILLER_2,
        GEN_QUIET,
        QUIET,
        END
    };
    class MovePicker {
    public:
        MovePicker() = default;
        ~MovePicker() = default;
        void init_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool for_qs);
        void setup_mp(const Board& board, move::Move tt_move, search::SearchStack *ss, bool skip_quiets);
        move::Move next_move();
        move::Move next(Board& board);
    private:
        MoveList moves;
        StaticVector<int, 256> scores;

        MoveList noisy_moves;
        StaticVector<int, 256> noisy_scores;
        MoveList quiet_moves;
        StaticVector<int, 256> quiet_scores;

        STAGE stage;
        bool no_quiets;
        int noisy_size;
        int quiet_size;
        move::Move hash_move;
        move::Move killers[2];

        int id;

        void score_moves(const Board &board, const move::Move& tt_move, const search::SearchStack *ss);
        void score_noisy(const Board& board);
        void score_quiets(const Board& board);
    };
}