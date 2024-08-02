#pragma once

#include <array>
#include <vector>
#include <chrono>
#include <span>
#include <thread>
#include <atomic>

#include "board/board.h"
#include "move.h"

namespace elixir::search {
    class SearchInfo {
      public:
        SearchInfo() = default;
        SearchInfo(int depth, bool timed)
            : nodes(0), depth(depth), seldepth(0), stopped(false), timed(timed),
              best_root_move(move::NO_MOVE) {}

        ~SearchInfo() = default;
        unsigned long long nodes;
        int depth;
        int seldepth;
        bool stopped;
        bool timed;
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

    bool SEE(const Board &board, const move::Move move, int threshold,
             const int see_values[7] = see_pieces);

    class ThreadData {
        public:
            ThreadData(Board& board, SearchInfo& info, int idx, std::atomic<unsigned long long>* nodes) {
                this->board = board;
                this->info = info;
                this->thread_idx = idx;
                this->nodes = nodes;
            }
            ~ThreadData() = default;

            int thread_idx = -1;
            Board board;
            SearchInfo info;

            std::atomic<unsigned long long> *nodes = nullptr;
    };

    class Searcher {
        History history;
        public:
        bool searching = false;
        int qsearch(ThreadData &td, int alpha, int beta, PVariation &pv,
            SearchStack *ss);
        int negamax(ThreadData &td, int alpha, int beta, int depth, PVariation &pv,
                    SearchStack *ss, bool cutnode);
        void search(ThreadData &td, bool print_info);
        void clear_history() { history.clear(); }
    };

    class ThreadManager {
        public:
        ThreadManager(int threads) {
            num_threads = threads;
            for (int i = 0; i < num_threads; i++) {
                searchers.push_back(Searcher());
            }
        }
        ~ThreadManager() = default;

        std::vector<Searcher> searchers;

        std::vector<std::jthread> threads;
        std::vector<ThreadData> thread_datas;

        int num_threads = 1;
        std::atomic<bool> in_search{false};
        std::atomic<unsigned long long> nodes{0};

        unsigned long long get_nodes() {
            unsigned long long nodes = 0;
            for (auto &td: thread_datas) {
                nodes += td.info.nodes;
            }
            return nodes;
        }

        void ucinewgame() { 
            for (int i = 0; i < num_threads; i++) {
                searchers[i].clear_history();
            }
        }

        void set_threads(int n) {
            num_threads = n;
            for (int i = 0; i < num_threads; i++) {
                searchers.push_back(Searcher());
            }
            threads.clear();
            thread_datas.clear();
            searchers.clear();
            for (int i = 0; i < num_threads; i++) {
                searchers.push_back(Searcher());
            }
        }

        void stop_search() {
            if (!in_search) return;
            for (int i = 0; i < num_threads; i++) {
                thread_datas[i].info.stopped = true;
                while (searchers[i].searching) {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
            }
        }

        void search(Board &board, SearchInfo &info, bool print_info = true);
    };

    inline ThreadManager main_searcher(1);
}
