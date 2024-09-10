#pragma once

#include <array>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

#include "board/board.h"
#include "move.h"

#include "search_terms.h"
#include "pv.h"

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

    void init_lmr();
    bool SEE(const Board &board, const move::Move move, int threshold,
             const int see_values[7] = see_pieces);

    class ThreadData {
        public:
            ThreadData(Board &board, SearchInfo &info) {
                this->board = board;
                this->info = info;
            }
            ~ThreadData() = default;

            int thread_idx = -1;
            Board board;
            SearchInfo info;
    };

    class Searcher {
        History history;
        public:
        bool searching = false;
        bool soft_stop = true;

        int futility_margin(int depth, bool improving, bool cutnode, bool tt_hit) const {
            int futilitity_base = 122 - 37 * (cutnode && !tt_hit);
            int improving_reduction = improving * futilitity_base / 2;

            return futilitity_base * depth - improving_reduction;
        }

        int qsearch(ThreadData &td, int alpha, int beta, PVariation &pv,
            SearchStack *ss);
        int negamax(ThreadData &td, int alpha, int beta, int depth, PVariation &pv,
                    SearchStack *ss, bool cutnode);
        void search(ThreadData &td, bool print_info);
        void clear_history() { history.clear(); }
        void update_killers_and_histories(SearchStack*ss, move::Move move, MoveList& bad_quiets, Color stm, int depth, bool is_quiet_move);
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

        std::vector<std::thread> threads;
        std::vector<ThreadData> thread_datas;

        int num_threads = 1;
        std::atomic<bool> in_search{false};

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
                searchers[i].soft_stop = false;
                thread_datas[i].info.stopped = true;
            }
        }

        void search(Board &board, SearchInfo &info, bool print_info = true);
    };

    inline ThreadManager main_searcher(DEFAULT_THREADS);
}
