#include <cassert>
#include <iostream>

#include "evaluate.h"

#include "board/board.h"
#include "defs.h"
#include "nnue/nnue.h"
#include "texel/texel.h"
#include "types.h"
#include "utils/bits.h"

using namespace elixir::bits;
using namespace elixir::texel;

namespace elixir::eval {
    int TEMPO           = 7;
    int MP_PAWN         = 95;
    int MP_KNIGHT       = 285;
    int MP_BISHOP       = 364;
    int MP_ROOK         = 487;
    int MP_QUEEN        = 906;
    int MP_KING         = 21029;
    int piece_values[7] = {MP_PAWN, MP_KNIGHT, MP_BISHOP, MP_ROOK, MP_QUEEN, MP_KING, 0};
}