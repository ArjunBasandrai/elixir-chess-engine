#pragma once

#include <array>
#include <chrono>
#include <span>

#include "board/board.h"
#include "move.h"

namespace elixir::search {
    class SearchInfo {
      public:
        SearchInfo() = default;
        SearchInfo(int depth)
            : nodes(0), stopped(false), timed(false),
              start_time(std::chrono::high_resolution_clock::now()), depth(depth), seldepth(0),
              soft_limit(0), hard_limit(0), best_root_move(move::NO_MOVE) {}
        SearchInfo(int depth, std::chrono::high_resolution_clock::time_point start_time,
                   F64 soft_limit, F64 hard_limit)
            : nodes(0), depth(depth), seldepth(0), stopped(false), timed(true),
              start_time(start_time), soft_limit(soft_limit), hard_limit(hard_limit),
              best_root_move(move::NO_MOVE) {}

        ~SearchInfo() = default;
        unsigned long long nodes;
        int depth;
        int seldepth;
        bool stopped;
        bool timed;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        F64 soft_limit;
        F64 hard_limit;
        move::Move best_root_move;
    };

    struct PVariation {
        std::size_t length;
        int score;
        std::array<move::Move, 256> line;

        PVariation() : length(0), score(0) {
            for (auto &move : line) {
                move = move::NO_MOVE;
            }
        }

        std::span<move::Move> moves() { return std::span<move::Move>(line.data(), length); }

        int score_value() const { return score; }

        void print_pv() const {
            for (int i = 0; i < length; i++) {
                line[i].print_uci();
                std::cout << " ";
            }
        }

        void update(const move::Move m, const int s, const PVariation &rest) {
            line[0] = m;
            std::copy(rest.line.begin(), rest.line.begin() + rest.length, line.begin() + 1);
            length = rest.length + 1;
            score  = s;
        }
    };

    extern int RFP_MARGIN;
    extern int LMP_BASE;
    extern int RAZOR_MARGIN;
    extern int NMP_BASE_REDUCTION;
    extern int NMP_DEPTH;
    extern int RFP_DEPTH;
    extern int RAZOR_DEPTH;
    extern int IIR_DEPTH;
    extern int LMP_MULTIPLIER;
    extern int LMR_DEPTH;
    extern int FP_BASE;
    extern int FP_MULTIPLIER;
    extern int FP_DEPTH;
    extern int SEE_QUIET;
    extern int SEE_CAPTURE;
    extern int SEE_DEPTH;
    extern int QS_SEE_THRESHOLD;
    extern int SEE_PAWN;
    extern int SEE_KNIGHT;
    extern int SEE_BISHOP;
    extern int SEE_ROOK;
    extern int SEE_QUEEN;
    extern int INITIAL_ASP_DELTA;
    extern int NMP_DIVISOR;
    extern double LMR_OFFSET;
    extern double LMR_DIVISOR;
    extern int MIN_ASP_DEPTH;
    extern double ASP_MULTIPLIER;

    extern int see_pieces[7];

    extern int lmr[MAX_DEPTH][64];
    void init_lmr();

    int negamax(Board &board, int alpha, int beta, int depth, SearchInfo &info, PVariation &pv,
                SearchStack *ss);
    bool SEE(const Board &board, const move::Move move, int threshold,
             const int see_values[7] = see_pieces);
    void search(Board &board, SearchInfo &info, bool print_info = true);
}
