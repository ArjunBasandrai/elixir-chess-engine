#include <cassert>

#include "board.h"

#include "types.h"
#include "defs.h"
#include "bits.h"

namespace elixir {
    void Board::set_piece(Square sq, PieceType piece, Color color) {
        U64 b_bit = bits::bit(sq);
        assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
        b_occupancies[static_cast<I8>(color)] |= b_bit;
        b_pieces[static_cast<I8>(piece)] |= b_bit;
    }
}