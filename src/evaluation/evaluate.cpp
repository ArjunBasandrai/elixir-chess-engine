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
    int MP_PAWN         = 100;
    int MP_KNIGHT       = 284;
    int MP_BISHOP       = 367;
    int MP_ROOK         = 489;
    int MP_QUEEN        = 902;
    int MP_KING         = 20936;
    int piece_values[7] = {MP_PAWN, MP_KNIGHT, MP_BISHOP, MP_ROOK, MP_QUEEN, MP_KING, 0};
}