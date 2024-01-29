#include "types.h"
#include "defs.h"
#include "move.h"

namespace elixir::move {
    class Move {
    public:
        [[nodiscard]] constexpr Move_T encode_move(Square from, Square to, Piece piece, Flag flag, Promotion promotion) {
            Move_T move = 0;
            move |= static_cast<Move_T>(from);
            move |= static_cast<Move_T>(to) << 6;
            move |= static_cast<Move_T>(piece) << 12;
            move |= static_cast<Move_T>(flag) << 16;
            move |= static_cast<Move_T>(promotion) << 19;
            return move;        
        }
        [[nodiscard]] constexpr Move_T encode_move(Square from, Square to, int piece, Flag flag, Promotion promotion) {
            Move_T move = 0;
            move |= static_cast<Move_T>(from);
            move |= static_cast<Move_T>(to) << 6;
            move |= static_cast<Move_T>(piece) << 12;
            move |= static_cast<Move_T>(flag) << 16;
            move |= static_cast<Move_T>(promotion) << 19;
            return move;        
        }
        [[nodiscard]] constexpr Move_T encode_move(int from, int to, Piece piece, Flag flag, Promotion promotion) {
            Move_T move = 0;
            move |= static_cast<Move_T>(from);
            move |= static_cast<Move_T>(to) << 6;
            move |= static_cast<Move_T>(piece) << 12;
            move |= static_cast<Move_T>(flag) << 16;
            move |= static_cast<Move_T>(promotion) << 19;
            return move;        
        }
        [[nodiscard]] constexpr Move_T encode_move(int from, int to, int piece, Flag flag, Promotion promotion) {
            Move_T move = 0;
            move |= static_cast<Move_T>(from);
            move |= static_cast<Move_T>(to) << 6;
            move |= static_cast<Move_T>(piece) << 12;
            move |= static_cast<Move_T>(flag) << 16;
            move |= static_cast<Move_T>(promotion) << 19;
            return move;        
        }
    };
}