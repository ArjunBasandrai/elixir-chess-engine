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
#include "utils/timer.h"
#include "search.h"
#include "tt.h"

#define version "0.1"

namespace elixir::uci {
    void parse_position(std::string input, Board &board) {
        if (input.substr(9, 8) == "startpos") {
            board.from_fen(start_position);
            if (input.length() > 23) {
                if (input.substr(18, 5) == "moves") {
                    std::string moves = input.substr(24);
                    std::vector<std::string> move_list = str_utils::split(moves, ' ');
                    for (auto move : move_list) {
                        board.parse_uci_move(move);
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
                        board.parse_uci_move(move);
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
        int depth = 64, movestogo = 20;
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
                } else if ((tokens[i] == "wtime" || tokens[i] == "btime") && ++i < tokens.size() && tokens[i - 1] == (board.get_side_to_move() == Color::WHITE ? "wtime" : "btime")) {
                    time = std::stoi(tokens[i]);
                    time = std::max<F64>(time, 1.0);
                } else if ((tokens[i] == "winc" || tokens[i] == "binc") && ++i < tokens.size() && tokens[i - 1] == (board.get_side_to_move() == Color::WHITE ? "winc" : "binc")) {
                    inc = std::stoi(tokens[i]);
                    inc = std::max<F64>(inc, 1.0);
                } else if (tokens[i] == "movestogo" && ++i < tokens.size()) {
                    movestogo = std::stoi(tokens[i]);
                    movestogo = std::min<int>(movestogo, static_cast<U32>(std::numeric_limits<I32>::max()));
                    if (!movestogo) movestogo = 20;
                }
            }
        }

        if (time != 0)
        {
            const F64 limit = std::max(0.001, time - MoveOverhead);
            const F64 search_time = (limit / static_cast<F64>(movestogo) + inc / IncrementScale);
            info = search::SearchInfo(depth, start_time, search_time);
        }
        else
        {
            info = search::SearchInfo(depth);
        }

        search::search(board, info);
    }

    void parse_setoption(std::string input)
    {
        std::vector<std::string> tokens = str_utils::split(input, ' ');
        if (tokens.size() < 3)
        {
            return;
        }
        if (tokens[1] == "name")
        {
            if (tokens[2] == "Hash")
            {
                if (tokens.size() < 5 || tokens[3] != "value")
                {
                    return;
                }
                int tt_size = std::stoi(tokens[4]);
                tt_size = std::clamp<int>(tt_size, MIN_HASH, MAX_HASH);
                tt->resize(tt_size);
            }
        }
    }

    void uci_loop(Board &board)
    {
        while (true)
        {
            std::string input;
            std::getline(std::cin, input);
            if (input == "uci")
            {
                std::cout << "id name Elixir " << version << std::endl;
                std::cout << "id author Arjun Basandrai" << std::endl;
                std::cout << "option name Hash type spin default " << DEFAULT_HASH_SIZE << " min " << MIN_HASH << " max " << MAX_HASH << std::endl;
                std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
                std::cout << "uciok" << std::endl;
            }
            else if (input == "isready")
            {
                std::cout << "readyok" << std::endl;
            }
            else if (input == "quit")
            {
                break;
            }
            else if (input == "ucinewgame")
            {
                board.from_fen(start_position);
                tt->clear_tt();
            }
            else if (input == "bench")
            {
                bench::bench();
                break;
            }
            else if (input == "print")
            {
                board.print_board();
                // print_bitboard(board.pawns());
                // print_bitboard(board.knights());
                // print_bitboard(board.bishops());
                // print_bitboard(board.rooks());
                // print_bitboard(board.queens());
                // print_bitboard(board.king());
                // print_bitboard(board.white_occupancy());
                // print_bitboard(board.black_occupancy());
            }
            else if (input.substr(0, 9) == "position ")
            {
                parse_position(input, board);
            }
            else if (input.substr(0, 2) == "go")
            {
                parse_go(input, board);
            }
            else if (input.substr(0, 9) == "setoption")
            {
                parse_setoption(input);
            }
        }
    }
}
