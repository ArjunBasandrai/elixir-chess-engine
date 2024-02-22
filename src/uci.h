#pragma once

#include <string>

#include "board/board.h"

namespace elixir::uci {
    void parse_go(std::string input, Board &board);
    void uci_loop(Board& board);
}
