#include <iostream>

#include "src/types.h"
#include "src/defs.h"
#include "src/utils/bits.h"
#include "src/utils/test_fens.h"
#include "src/board/board.h"

using namespace elixir;

int main() {
    Board board;
    board.from_fen(custom_position);
    board.print_board();
    board.from_fen(start_position);
    board.print_board();
    return 0;
}