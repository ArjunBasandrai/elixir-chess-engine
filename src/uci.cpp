#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include "uci.h"

#include "bench/bench.h"
#include "utils/test_fens.h"
#include "utils/str_utils.h"
#include "utils/perft.h"
#include "tests/see_test.h"
#include "search.h"
#include "tt.h"
#include "movepicker.h"
#include "history.h"

#define version "0.1"

namespace elixir::uci {
    int DEFAULT_MOVESTOGO = 20;
    int DEFAULT_MOVE_OVERHEAD = 50;
    int INCREMENT_SCALE = 2;

    void parse_position(std::string input, Board &board) {
        if (input.substr(9, 8) == "startpos") {
            board.from_fen(start_position);
            if (input.length() > 23) {
                if (input.substr(18, 5) == "moves") {
                    std::string moves = input.substr(24);
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
                    std::string fen = input.substr(fen_pos + 4, moves_pos - (fen_pos + 5)); // Offset by 4 to skip "fen " and subtract 5 to exclude the space before "moves"
                    board.from_fen(fen);
                    std::string moves = input.substr(moves_pos + 6);
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

    void parse_go(std::string input, Board &board) {
        std::vector<std::string> tokens = str_utils::split(input, ' ');
        search::SearchInfo info;
        const auto start_time = std::chrono::high_resolution_clock::now();
        int depth = MAX_DEPTH, movestogo = DEFAULT_MOVESTOGO;
        F64 time = 0, inc = 0;
        // If there are no tokens after "go" command, return
        if (tokens.size() <= 1) return;

        for (int i = 1; i < tokens.size(); i++) {
            if (tokens[i] == "depth") {
                // If depth is not specified, return
                if (++i >= tokens.size()) return;
                depth = std::stoi(tokens[i]);
            } else if (tokens[i] == "perft") {
                const int depth = std::stoi(tokens[++i]);
                long long nodes = 0;
                perft_driver(board, depth, nodes);
                std::cout << "Nodes: " << nodes << std::endl;
                return;
            } else {
                if (tokens[i] == "infinite") {
                    search::SearchInfo info(MAX_DEPTH);
                } else if ((tokens[i] == "wtime" || tokens[i] == "btime") && ++i < (int)tokens.size() && tokens[i - 1] == (board.get_side_to_move() == Color::WHITE ? "wtime" : "btime")) {
                    time = std::stoi(tokens[i]);
                    time = std::max<F64>(time, 1.0);
                } else if ((tokens[i] == "winc" || tokens[i] == "binc") && ++i < (int)tokens.size() && tokens[i - 1] == (board.get_side_to_move() == Color::WHITE ? "winc" : "binc")) {
                    inc = std::stoi(tokens[i]);
                    inc = std::max<F64>(inc, 1.0);
                } else if (tokens[i] == "movestogo" && ++i < tokens.size()) {
                    movestogo = std::stoi(tokens[i]);
                    movestogo = std::min<int>(movestogo, static_cast<U32>(std::numeric_limits<I32>::max()));
                    if (!movestogo) movestogo = DEFAULT_MOVESTOGO;
                }
            }
        }

        if (time != 0) {
            const F64 limit = std::max(0.001, time - DEFAULT_MOVE_OVERHEAD);
            const F64 search_time = (limit / static_cast<F64>(movestogo) + inc / INCREMENT_SCALE);
            info = search::SearchInfo(depth, start_time, search_time);
        }
        else {
            info = search::SearchInfo(depth);
        }

        search::search(board, info);
    }

    void parse_setoption(std::string input) {
        std::vector<std::string> tokens = str_utils::split(input, ' ');

        if (tokens.size() < 5 || tokens[3] != "value") return;

        if (tokens[1] == "name") {
            std::string option_value = tokens[4];
            if (tokens[2] == "Hash") {
                int tt_size = std::stoi(option_value);
                tt_size = std::clamp<int>(tt_size, MIN_HASH, MAX_HASH);
                tt->resize(tt_size);
            }

            else if (tokens[2] == "LMP_BASE") {
                int lmp_base = std::stoi(option_value);
                search::LMP_BASE = lmp_base;
            }

            else if (tokens[2] == "RFP_MARGIN") {
                int rfp_margin = std::stoi(option_value);
                search::RFP_MARGIN = rfp_margin;
            }

            else if (tokens[2] == "RAZOR_MARGIN") {
                int razor_margin = std::stoi(option_value);
                search::RAZOR_MARGIN = razor_margin;
            }

            else if (tokens[2] == "NMP_BASE_REDUCTION") {
                int nmp_base_reduction = std::stoi(option_value);
                search::NMP_BASE_REDUCTION = nmp_base_reduction;
            }

            else if (tokens[2] == "NMP_DEPTH") {
                int nmp_depth = std::stoi(option_value);
                search::NMP_DEPTH = nmp_depth;
            }

            else if (tokens[2] == "NMP_DIVISOR") {
                int nmp_divisor = std::stoi(option_value);
                search::NMP_DIVISOR = nmp_divisor;
            }

            else if (tokens[2] == "RFP_DEPTH") {
                int rfp_depth = std::stoi(option_value);
                search::RFP_DEPTH = rfp_depth;
            }

            else if (tokens[2] == "RAZOR_DEPTH") {
                int razor_depth = std::stoi(option_value);
                search::RAZOR_DEPTH = razor_depth;
            }

            else if (tokens[2] == "IIR_DEPTH") {
                int iir_depth = std::stoi(option_value);
                search::IIR_DEPTH = iir_depth;
            }

            else if (tokens[2] == "LMP_MULTIPLIER") {
                int lmp_multiplier = std::stoi(option_value);
                search::LMP_MULTIPLIER = lmp_multiplier;
            }

            else if (tokens[2] == "LMR_DEPTH") {
                int lmr_depth = std::stoi(option_value);
                search::LMR_DEPTH = lmr_depth;
            }

            else if (tokens[2] == "LMR_OFFSET") {
                float lmr_offset = std::stof(option_value);
                search::LMR_OFFSET = lmr_offset;
                search::init_lmr();
            }

            else if (tokens[2] == "LMR_DIVISOR") {
                float lmr_divisor = std::stof(option_value);
                search::LMR_DIVISOR = lmr_divisor;
                search::init_lmr();
            }

            else if (tokens[2] == "FP_BASE") {
                int fp_base = std::stoi(option_value);
                search::FP_BASE = fp_base;
            }

            else if (tokens[2] == "FP_MULTIPLIER") {
                int fp_multiplier = std::stoi(option_value);
                search::FP_MULTIPLIER = fp_multiplier;
            }

            else if (tokens[2] == "FP_DEPTH") {
                int fp_depth = std::stoi(option_value);
                search::FP_DEPTH = fp_depth;
            }

            else if (tokens[2] == "SEE_QUIET") {
                int see_quiet = std::stoi(option_value);
                search::SEE_QUIET = see_quiet;
            }

            else if (tokens[2] == "SEE_CAPTURE") {
                int see_capture = std::stoi(option_value);
                search::SEE_CAPTURE = see_capture;
            }

            else if (tokens[2] == "SEE_DEPTH") {
                int see_depth = std::stoi(option_value);
                search::SEE_DEPTH = see_depth;
            }

            else if (tokens[2] == "QS_SEE_THRESHOLD") {
                int qs_see_threshold = std::stoi(option_value);
                search::QS_SEE_THRESHOLD = qs_see_threshold;
            }

            else if (tokens[2] == "SEE_PAWN") {
                int see_pawn = std::stoi(option_value);
                search::SEE_PAWN = see_pawn;
            }

            else if (tokens[2] == "SEE_KNIGHT") {
                int see_knight = std::stoi(option_value);
                search::SEE_KNIGHT = see_knight;
            }

            else if (tokens[2] == "SEE_BISHOP") {
                int see_bishop = std::stoi(option_value);
                search::SEE_BISHOP = see_bishop;
            }

            else if (tokens[2] == "SEE_ROOK") {
                int see_rook = std::stoi(option_value);
                search::SEE_ROOK = see_rook;
            }

            else if (tokens[2] == "SEE_QUEEN") {
                int see_queen = std::stoi(option_value);
                search::SEE_QUEEN = see_queen;
            }

            else if (tokens[2] == "MP_SEE") {
                int mp_see = std::stoi(option_value);
                MP_SEE = mp_see;
            }

            else if (tokens[2] == "MIN_ASP_DEPTH") {
                int min_asp_depth = std::stoi(option_value);
                search::MIN_ASP_DEPTH = min_asp_depth;
            }

            else if (tokens[2] == "DEFAULT_MOVESTOGO") {
                int default_movestogo = std::stoi(option_value);
                DEFAULT_MOVESTOGO = default_movestogo;
            }

            else if (tokens[2] == "DEFAULT_MOVE_OVERHEAD") {
                int default_move_overhead = std::stoi(option_value);
                DEFAULT_MOVE_OVERHEAD = default_move_overhead;
            }

            else if (tokens[2] == "INCREMENT_SCALE") {
                int increment_scale = std::stoi(option_value);
                INCREMENT_SCALE = increment_scale;
            }

            else if (tokens[2] == "HISTORY_GRAVITY") {
                int history_gravity = std::stoi(option_value);
                HISTORY_GRAVITY = history_gravity;
            }

        }
    }

    void uci_loop(Board &board) {
        while (true) {
            std::string input;
            std::getline(std::cin, input);
            if (input == "uci") {
                std::cout << "id name Elixir " << version << std::endl;
                std::cout << "id author Arjun Basandrai" << std::endl;
                std::cout << "option name Hash type spin default " << DEFAULT_HASH_SIZE << " min " << MIN_HASH << " max " << MAX_HASH << std::endl;
                std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
                std::cout << "option name LMP_BASE type spin default 7 min 2 max 14" << std::endl;
                std::cout << "option name RFP_MARGIN type spin default 100 min 20 max 350" << std::endl;
                std::cout << "option name RAZOR_MARGIN type spin default 264 min 100 max 400" << std::endl;
                std::cout << "option name NMP_BASE_REDUCTION type spin default 5 min 2 max 10" << std::endl;
                std::cout << "option name NMP_DEPTH type spin default 2 min 1 max 6" << std::endl;
                std::cout << "option name RFP_DEPTH type spin default 6 min 2 max 10" << std::endl;
                std::cout << "option name RAZOR_DEPTH type spin default 6 min 2 max 10" << std::endl;
                std::cout << "option name IIR_DEPTH type spin default 4 min 1 max 8" << std::endl;
                std::cout << "option name LMP_MULTIPLIER type spin default 1 min 1 max 8" << std::endl;
                std::cout << "option name LMR_DEPTH type spin default 3 min 1 max 8" << std::endl;
                std::cout << "option name FP_BASE type spin default 150 min 20 max 500" << std::endl;
                std::cout << "option name FP_MULTIPLIER type spin default 100 min 20 max 400" << std::endl;
                std::cout << "option name FP_DEPTH type spin default 8 min 1 max 15" << std::endl;
                std::cout << "option name SEE_QUIET type spin default 80 min 5 max 200" << std::endl;
                std::cout << "option name SEE_CAPTURE type spin default 30 min 5 max 200" << std::endl;
                std::cout << "option name SEE_DEPTH type spin default 8 min 1 max 15" << std::endl;
                std::cout << "option name QS_SEE_THRESHOLD type spin default 7 min 1 max 100" << std::endl;
                std::cout << "option name SEE_PAWN type spin default 100 min 20 max 300" << std::endl;
                std::cout << "option name SEE_KNIGHT type spin default 300 min 120 max 500" << std::endl;
                std::cout << "option name SEE_BISHOP type spin default 300 min 120 max 500" << std::endl;
                std::cout << "option name SEE_ROOK type spin default 500 min 300 max 700" << std::endl;
                std::cout << "option name SEE_QUEEN type spin default 900 min 500 max 1300" << std::endl;
                std::cout << "option name MP_SEE type spin default 107 min 20 max 500" << std::endl;
                std::cout << "option name DEFAULT_MOVESTOGO type spin default 20 min 5 max 50" << std::endl;
                std::cout << "option name DEFAULT_MOVE_OVERHEAD type spin default 50 min 5 max 150" << std::endl;
                std::cout << "option name INCREMENT_SCALE type spin default 2 min 1 max 10" << std::endl;
                std::cout << "option name HISTORY_GRAVITY type spin default 8192 min 1024 max 16384" << std::endl;
                std::cout << "option name NMP_DIVISOR type spin default 6 min 1 max 14" << std::endl;
                std::cout << "option name MIN_ASP_DEPTH type spin default 4 min 1 max 10" << std::endl;
                std::cout << "option name LMR_OFFSET type string default 0.75 min 0.0 max 4.0" << std::endl;
                std::cout << "option name LMR_DIVISOR type string default 2.31 min 0.5 max 10.0" << std::endl;
                std::cout << "uciok" << std::endl;
            }
            else if (input == "isready") {
                std::cout << "readyok" << std::endl;
            }
            else if (input == "quit") {
                break;
            }
            else if (input == "ucinewgame") {
                board.from_fen(start_position);
                tt->clear_tt();
            }
            else if (input == "bench"){
                 bench::bench();
                break;
            }
            else if (input == "see") {
                tests::see_test();
                break;
            }
            else if (input == "print") {
                board.print_board();
            }
            else if (input.substr(0, 9) == "position ") {
                parse_position(input, board);
            }
            else if (input.substr(0, 2) == "go") {
                parse_go(input, board);
            }
            else if (input.substr(0, 9) == "setoption") {
                parse_setoption(input);
            }
        }
    }
}
