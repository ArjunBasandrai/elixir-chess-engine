#include "eval_masks.h"

namespace elixir {
    Bitboard isolated_pawn_mask[8];
    void init_masks() {
        for (int i = 0; i < 8; i++) {
            isolated_pawn_mask[i] = 0ULL;
            if (i != 0) isolated_pawn_mask[i] |= (File_A_BB << (i - 1));
            if (i != 7) isolated_pawn_mask[i] |= (File_A_BB << (i + 1));
        }
    }
}