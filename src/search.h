#pragma once

#include <array>
#include <span>
#include <chrono>

#include "board/board.h"
#include "move.h"

namespace elixir::search {
    class SearchInfo {
    public:
        SearchInfo() = default;
        SearchInfo(int depth) : depth(depth), nodes(0), ply(0), stopped(false), timed(false), x(0), start_time(0), time_left(0) {};
        SearchInfo(int depth, F64 start_time, F64 time_left) : depth(depth), nodes(0), ply(0), stopped(false), timed(true), x(0), start_time(start_time), time_left(time_left) {};
        ~SearchInfo() = default;
        unsigned long long nodes;
        int depth;
        int ply;
        bool stopped;
        bool timed;
        int x;
        F64 start_time;
        F64 time_left;
    };
    
    struct PVariation {
        std::size_t length;
        int score;
        std::array<move::Move, MAX_PLY> line;

        PVariation() : length(0), score(0) {
            for (auto& move : line) {
                move = move::Move();
            }
        }

        std::span<move::Move> moves() {
            return std::span<move::Move>(line.data(), length);
        }

        int score_value() const {
            return score;
        }

        void print_pv() const {
            for (int i = 0; i < length; i++) {
                line[i].print_uci();
                std::cout << " ";
            }
        }

        void load_from(move::Move m, const PVariation& rest) {
            line[0] = m;
            std::copy(rest.line.begin(), rest.line.begin() + rest.length, line.begin() + 1);
            length = rest.length + 1;
        }
    };
    
    int negamax(Board& board, int alpha, int beta, int depth, SearchInfo& info, PVariation& pv);
    inline void search(Board& board, SearchInfo& info) {
        PVariation pv;
        int score = 0, alpha = -50000, beta = 50000;
        auto start = std::chrono::high_resolution_clock::now();
        for (int current_depth = 1; current_depth <= info.depth; current_depth++) {
            if (info.stopped) { break; }
            score = negamax(board, -50000, 50000, current_depth, info, pv);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << info.nodes << " time " << duration.count() << " pv ";
            pv.print_pv();
            std::cout << std::endl;
        }
        std::cout << "bestmove ";
        pv.line[0].print_uci();
        std::cout << std::endl;
    }
}
