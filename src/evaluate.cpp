#include <cassert>
#include <iostream>

#include "evaluate.h"

#include "board/board.h"
#include "defs.h"
#include "nnue/nnue.h"
#include "texel/texel.h"
#include "types.h"
#include "utils/bits.h"
#include "utils/eval_terms.h"
#include "utils/masks.h"

using namespace elixir::bits;
using namespace elixir::texel;

namespace elixir::eval {
    int TEMPO           = 7;
    int MP_PAWN         = 86;
    int MP_KNIGHT       = 304;
    int MP_BISHOP       = 360;
    int MP_ROOK         = 466;
    int MP_QUEEN        = 905;
    int MP_KING         = 20903;
    int piece_values[7] = {MP_PAWN, MP_KNIGHT, MP_BISHOP, MP_ROOK, MP_QUEEN, MP_KING, 0};

    int evaluate(const Board &board) {
        const int eval =  nn.eval(board.get_side_to_move());
        const int phase = 3 * count_bits(board.knights()) + 3 * count_bits(board.bishops()) + 5 * count_bits(board.rooks()) + 10 * count_bits(board.queens());
        const int scaled_eval = eval * (phase + 206) / 256;
        return scaled_eval;
    }
}