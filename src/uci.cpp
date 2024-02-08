#include <iostream>
#include <string>
#include <vector>

#include "uci.h"

#include "utils/test_fens.h"
#include "utils/str_utils.h"

#define version "0.1"

namespace elixir::uci {
    void parse_position(std::string input, Board &board) {
        if (input.substr(9, 8) == "startpos") {
            board.from_fen(start_position);
            if (input.substr(18, 5) == "moves" && input.length() > 23) {
                std::string moves = input.substr(24);
                std::vector<std::string> move_list = str_utils::split(moves, ' ');
                for (auto move: move_list) {
                    board.parse_uci_move(move);
                }
            }
        } else if (input.substr(9, 3) == "fen") {
            std::string fen = input.substr(18);
            board.from_fen(fen);
        }
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
            } 
            else if (input.substr(0, 9) == "position ") {
                parse_position(input, board);
            }
            // } else if (input.substr(0, 2) == "go") {
        }
    }
}
