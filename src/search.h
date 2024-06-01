#pragma once

#include <array>
#include <span>
#include <chrono>

#include "board/board.h"
#include "move.h"

namespace elixir::search
{
    struct SearchStack
    {
        move::Move move = move::NO_MOVE;
        move::Move killers[2] = {};
        int eval;
        int ply;
    };

    class SearchInfo
    {
    public:
        SearchInfo() = default;
        SearchInfo(int depth)
        : nodes(0),
          stopped(false),
          timed(false),
          start_time(std::chrono::high_resolution_clock::now()),
          depth(depth),
          time_left(0) {}
        SearchInfo(int depth, std::chrono::high_resolution_clock::time_point start_time, double time_left)
        : nodes(0),
          depth(depth),
          stopped(false),
          timed(true),
          start_time(start_time),
          time_left(time_left) {}

        ~SearchInfo() = default;
        unsigned long long nodes;
        int depth;
        bool stopped;
        bool timed;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        F64 time_left;
    };

    struct PVariation
    {
        std::size_t length;
        int score;
        std::array<move::Move, 256> line;

        PVariation() : length(0), score(0)
        {
            for (auto &move : line)
            {
                move = move::NO_MOVE;
            }
        }

        std::span<move::Move> moves()
        {
            return std::span<move::Move>(line.data(), length);
        }

        int score_value() const
        {
            return score;
        }

        void print_pv() const
        {
            for (int i = 0; i < length; i++)
            {
                line[i].print_uci();
                std::cout << " ";
            }
        }

        void update(const move::Move m, const int s, const PVariation &rest)
        {
            line[0] = m;
            std::copy(rest.line.begin(), rest.line.begin() + rest.length, line.begin() + 1);
            length = rest.length + 1;
            score = s;
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

    extern int lmr[MAX_DEPTH][64];
    void init_lmr();

    static constexpr int see_values[7] = {
        100, 450, 450, 650, 1250, 0, 0
    };

    int negamax(Board &board, int alpha, int beta, int depth, SearchInfo &info, PVariation &pv, SearchStack *ss);
    bool SEE(const Board& board, const move::Move move, int threshold);
    void search(Board &board, SearchInfo &info, bool print_info = true);
}
