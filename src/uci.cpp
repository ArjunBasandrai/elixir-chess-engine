#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "uci.h"

#include "utils/test_fens.h"
#include "utils/str_utils.h"
#include "utils/timer.h"
#include "search.h"

#define version "0.1"

namespace elixir::uci {
    void parse_position(std::string input, Board &board) {
        if (input.substr(9, 8) == "startpos") {
            board.from_fen(start_position);
            if (input.length() > 23) {
                if (input.substr(18, 5) == "moves") {
                    std::string moves = input.substr(24);
                    std::vector<std::string> move_list = str_utils::split(moves, ' ');
                    for (auto move: move_list) {
                        board.parse_uci_move(move);
                    }
                }
            }
        } else if (input.substr(9, 3) == "fen") {
            std::string fen = input.substr(13);
            board.from_fen(fen);
        }
    }

    void parse_go(std::string input, Board &board) {
        std::vector<std::string> tokens = str_utils::split(input, ' ');
        search::SearchInfo info;
        const auto start_time = timer::m_timer.time();
        int depth = 64, movestogo = 20;
        F64 time = 0, inc = 0;
        for (int i = 0; i < tokens.size(); i++) {
             if (tokens[i] == "depth") {
                depth = std::stoi(tokens[++i]);
            } else {
                if (tokens[i] == "infinite") {
                    search::SearchInfo info(MAX_DEPTH);
                } else if ((tokens[i] == "wtime" || tokens[i] == "btime") && ++i < tokens.size() 
                    && tokens[i-1] == (board.get_side_to_move() == Color::WHITE ? "wtime" : "btime"))  {
                    time = std::stoi(tokens[i]);
                    time = std::max<F64>(time, 1.0);
                } else if ((tokens[i] == "winc" || tokens[i] == "binc") && ++i < tokens.size() 
                    && tokens[i-1] == (board.get_side_to_move() == Color::WHITE ? "winc" : "binc"))  {
                    inc = std::stoi(tokens[i]);
                    inc = std::max<F64>(inc, 1.0);
                } else if (tokens[i] == "movestogo"  && ++i < tokens.size()) {
                    movestogo = std::stoi(tokens[i]);
                    movestogo = std::min<int>(movestogo, static_cast<U32>(std::numeric_limits<I32>::max()));
                    if (movestogo == 0) {
                        movestogo = 20;
                    }
                }
            }
        }

        if (time != 0) {
            const F64 limit = std::max(0.001, time - MoveOverhead);
            const F64 search_time = (limit / static_cast<F64>(movestogo) + inc / IncrementScale);
            info = search::SearchInfo(depth, start_time, search_time);
        } else {
            info = search::SearchInfo(depth);
        }

        search::search(board, info);
    }

    void uci_loop(Board &board) {
        while (true) {
            std::string input;
            std::getline(std::cin, input);
            if (input == "uci") {
                std::cout << "id name Elixir " << version << std::endl;
                std::cout << "id author Arjun Basandrai" << std::endl;
                std::cout << "uciok" << std::endl;
            } else if (input == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (input == "quit") {
                break;
            } else if (input == "ucinewgame") {
                board.from_fen(start_position);
            } else if (input == "print") {
                board.print_board();
                print_bitboard(board.pawns());
                print_bitboard(board.knights());
                print_bitboard(board.bishops());
                print_bitboard(board.rooks());
                print_bitboard(board.queens());
                print_bitboard(board.king());
                print_bitboard(board.white_occupancy());
                print_bitboard(board.black_occupancy());
            } else if (input.substr(0, 9) == "position ") {
                parse_position(input, board);
            } else if (input.substr(0, 2) == "go") {
                parse_go(input, board);
            }
        }
    }
}
