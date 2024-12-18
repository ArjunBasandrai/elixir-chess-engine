#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "uci.h"

#include "evaluation/evaluate.h"
#include "movegen.h"
#include "search/bench/bench.h"
#include "search/history/history.h"
#include "search/movepicker.h"
#include "search/search.h"
#include "spsa/spsa.h"
#include "tests/see_test.h"
#include "time_manager/time_management_terms.h"
#include "time_manager/time_manager.h"
#include "tt/tt.h"
#include "utils/perft.h"
#include "utils/str_utils.h"
#include "utils/test_fens.h"

#define version "3.0"

namespace elixir::uci {

    std::thread main_search_thread;
    std::condition_variable cv;
    std::mutex mtx;
    std::atomic<bool> ready{false};
    std::atomic<bool> exit_thread{false};
    std::function<void()> task;

    void search_thread_function() {
        std::unique_lock<std::mutex> lock(mtx);
        while (! exit_thread.load()) {
            cv.wait(lock, [] { return ready.load() || exit_thread.load(); });

            if (ready) {
                if (task) {
                    task();
                }
                ready = false;
            }
        }
    }

    void initiate_search(std::function<void()> search_task) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            task  = std::move(search_task);
            ready = true;
        }
        cv.notify_one();
    }

    void stop_search_thread(search::SearchInfo &info) {
        search::main_searcher.stop_search();
        info.stopped = true;
    }

    void exit() {
        exit_thread = true;
        cv.notify_one();
        search::main_searcher.stop_search();
        main_search_thread.join();
    }

    void parse_position(std::string input, Board &board) {
        if (input.substr(9, 8) == "startpos") {
            board.from_fen(start_position);
            if (input.length() > 23) {
                if (input.substr(18, 5) == "moves") {
                    std::string moves                  = input.substr(24);
                    std::vector<std::string> move_list = str_utils::split(moves, ' ');
                    for (auto move : move_list) {
                        board.play_uci_move(move);
                    }
                }
            }
        } else if (input.substr(9, 3) == "fen" && input.length() > 13) {
            size_t fen_pos = input.find("fen");
            if (fen_pos != std::string::npos) {
                size_t moves_pos = input.find("moves", fen_pos);
                if (moves_pos != std::string::npos) {
                    std::string fen =
                        input.substr(fen_pos + 4,
                                     moves_pos -
                                         (fen_pos + 5)); // Offset by 4 to skip "fen " and subtract
                                                         // 5 to exclude the space before "moves"
                    board.from_fen(fen);
                    std::string moves                  = input.substr(moves_pos + 6);
                    std::vector<std::string> move_list = str_utils::split(moves, ' ');
                    for (auto move : move_list) {
                        board.play_uci_move(move);
                    }
                } else {
                    std::string fen = input.substr(fen_pos + 4);
                    board.from_fen(fen);
                }
            }
        }
    }

    void parse_go(std::string input, Board &board, search::SearchInfo &info) {
        std::vector<std::string> tokens = str_utils::split(input, ' ');
        info                            = search::SearchInfo();
        const auto start_time           = std::chrono::high_resolution_clock::now();
        int depth = MAX_DEPTH, movestogo = -1;
        F64 time = 0, inc = 0;
        // If there are no tokens after "go" command, return
        if (tokens.size() <= 1)
            return;

        bool is_movetime = false;

        for (int i = 1; i < tokens.size(); i++) {
            if (tokens[i] == "depth") {
                // If depth is not specified, return
                if (++i >= tokens.size())
                    return;
                depth = std::stoi(tokens[i]);
            } else if (tokens[i] == "perft") {
                const int depth = std::stoi(tokens[++i]);
                long long nodes = 0;
                perft_driver(board, depth, nodes);
                std::cout << "Nodes: " << nodes << std::endl;
                return;
            } else {
                if (tokens[i] == "infinite") {
                    search::SearchInfo info(MAX_DEPTH, false);
                } else if ((tokens[i] == "wtime" || tokens[i] == "btime") &&
                           ++i < (int)tokens.size() &&
                           tokens[i - 1] ==
                               (board.get_side_to_move() == Color::WHITE ? "wtime" : "btime")) {
                    time = std::stoi(tokens[i]);
                    time = std::max<F64>(time, 1.0);
                } else if (tokens[i] == "movetime") {
                    time        = std::stoi(tokens[++i]);
                    time        = std::max<F64>(time, 1.0);
                    is_movetime = true;
                } else if ((tokens[i] == "winc" || tokens[i] == "binc") &&
                           ++i < (int)tokens.size() &&
                           tokens[i - 1] ==
                               (board.get_side_to_move() == Color::WHITE ? "winc" : "binc")) {
                    inc = std::stoi(tokens[i]);
                    inc = std::max<F64>(inc, 1.0);
                } else if (tokens[i] == "movestogo" && ++i < tokens.size()) {
                    movestogo = std::stoi(tokens[i]);
                    movestogo =
                        std::min<int>(movestogo, static_cast<U32>(std::numeric_limits<I32>::max()));
                    if (! movestogo)
                        movestogo = DEFAULT_MOVESTOGO;
                }
            }
        }

        if (time != 0) {
            time_manager.optimum_time(info, time, inc, movestogo, start_time, is_movetime);
        } else {
            info = search::SearchInfo(depth, false);
        }

        initiate_search([&]() { search::main_searcher.search(board, info); });
    }

    void parse_setoption(std::string input) {
        std::vector<std::string> tokens = str_utils::split(input, ' ');

        if (tokens.size() < 5 || tokens[3] != "value")
            return;

        if (tokens[1] == "name") {
            std::string option_value = tokens[4];
            if (tokens[2] == "Hash") {
                int tt_size = std::stoi(option_value);
                tt_size     = std::clamp<int>(tt_size, MIN_HASH, MAX_HASH);
                tt->resize(tt_size);
            } else if (tokens[2] == "Threads") {
                int thread_count = std::stoi(option_value);
                thread_count     = std::clamp<int>(thread_count, MIN_THREADS, MAX_THREADS);
                search::main_searcher.set_threads(thread_count);
            }

            else {
#ifdef USE_TUNE
                tune::tuner.update_parameter(tokens[2], option_value);
#endif
            }
        }
    }

    void uci_loop(Board &board) {
        main_search_thread = std::thread(search_thread_function);
        search::SearchInfo info;
        while (true) {
            std::string input;
            std::getline(std::cin, input);
            if (input == "uci") {
                std::cout << "id name Elixir " << version << std::endl;
                std::cout << "id author Arjun Basandrai" << std::endl;
                std::cout << "option name Hash type spin default " << DEFAULT_HASH_SIZE << " min "
                          << MIN_HASH << " max " << MAX_HASH << std::endl;
                std::cout << "option name Threads type spin default " << DEFAULT_THREADS << " min "
                          << MIN_THREADS << " max " << MAX_THREADS << std::endl;
#ifdef USE_TUNE
                tune::tuner.print_info();
#endif
                std::cout << "uciok" << std::endl;
            } else if (input == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (input == "quit") {
                stop_search_thread(info);
                exit();
                break;
            } else if (input == "stop") {
                stop_search_thread(info);
            } else if (input == "ucinewgame") {
                search::main_searcher.ucinewgame();
                board.to_startpos();
                tt->clear_tt();
            } else if (input == "bench") {
                bench::bench();
                exit();
                break;
            } else if (input == "see") {
                tests::see_test();
                exit();
                break;
            } else if (input == "print") {
                board.print_board();
            } else if (input == "legals") {
                auto moves = movegen::generate_moves<false>(board);
                for (auto move : moves) {
                    move.print_uci();
                    std::cout << std::endl;
                }
            } else if (input.substr(0, 9) == "position ") {
                parse_position(input, board);
            } else if (input.substr(0, 2) == "go") {
                parse_go(input, board, info);
            } else if (input.substr(0, 9) == "setoption") {
                parse_setoption(input);
            }
        }
    }
}
