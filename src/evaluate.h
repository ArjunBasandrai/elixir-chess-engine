#pragma once

#include "board/board.h"
#include "evaluate.h"
#include "types.h"

namespace elixir {

    constexpr EvalScore S(const Score o_score, const Score e_score) {
        return static_cast<EvalScore>(((I32)e_score << 16) + (I32)o_score);
    }

    constexpr Score O(const EvalScore score) {
        return static_cast<Score>((I16)score);
    }

    constexpr Score E(const EvalScore score) {
        return static_cast<Score>((I16)((score + 0x8000) >> 16));
    }
    namespace eval {
        extern int TEMPO;
        extern int MP_PAWN;
        extern int MP_KNIGHT;
        extern int MP_BISHOP;
        extern int MP_ROOK;
        extern int MP_QUEEN;
        extern int MP_KING;
        extern int piece_values[7];
        struct EvalInfo {
            EvalInfo() = default;
            EvalInfo(EvalScore score) : score(score) {}
            EvalScore score = 0;
            std::array<Bitboard, 2> king_zones;
            void add_score(EvalScore s) { score += s; }
            Score opening_score() { return O(score); }
            Score endgame_score() { return E(score); }
        };

        class Evaluator {
          public:
            Evaluator()  = default;
            ~Evaluator() = default;
            int evaluate(const Board &board);

          private:
            EvalInfo e_info;

            void init_evaluator(const Board &board);
            void evaluate_pawns(const Board &board, const Color side);
            void evaluate_knights(const Board &board, const Color side);
            void evaluate_bishops(const Board &board, const Color side);
            void evaluate_rooks(const Board &board, const Color side);
            void evaluate_queens(const Board &board, const Color side);
            void evaluate_kings(const Board &board, const Color side);
        };

        inline Evaluator evaluator;

        extern int evaluate(const Board &board);
    }
}