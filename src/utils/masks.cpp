#include "../types.h"
#include "../defs.h"
#include "bits.h"

#include "masks.h"

namespace elixir {
    Bitboard connected_pawn_masks[2][64];
    void init_masks() {
        for (int i = 8; i < 56; i++) {
            if (get_file(sq(i)) == 0) {
                connected_pawn_masks[0][i] = bits::bit(sq(i + 1)) | bits::bit(sq(i - 7));
                connected_pawn_masks[1][i] = bits::bit(sq(i + 1)) | bits::bit(sq(i + 9));
            }
            else if (get_file(sq(i)) == 7) {
                connected_pawn_masks[0][i] = bits::bit(sq(i - 1)) | bits::bit(sq(i - 9));
                connected_pawn_masks[1][i] = bits::bit(sq(i - 1)) | bits::bit(sq(i + 7));
            }
            else {
                connected_pawn_masks[0][i] = bits::bit(sq(i + 1)) | bits::bit(sq(i - 1)) | bits::bit(sq(i - 7)) | bits::bit(sq(i - 9));
                connected_pawn_masks[1][i] = bits::bit(sq(i + 1)) | bits::bit(sq(i - 1)) | bits::bit(sq(i + 7)) | bits::bit(sq(i + 9));
            }
        }
    }
}