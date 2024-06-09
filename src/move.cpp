#include <iostream>

#include "board/board.h"
#include "defs.h"
#include "move.h"
#include "types.h"

namespace elixir::move {
    Move::Move(Move_T move) {
        set_move(move);
    }

    Move::Move(Square from, Square to, Piece piece, Flag flag, Promotion promotion) {
        set_move(from, to, piece, flag, promotion);
    }

    [[nodiscard]] Move_T Move::encode_move(Square from, Square to, Piece piece, Flag flag,
                                           Promotion promotion) const noexcept {
        Move_T move = 0;
        move |= static_cast<Move_T>(from);
        move |= static_cast<Move_T>(to) << 6;
        move |= static_cast<Move_T>(piece) << 12;
        move |= static_cast<Move_T>(flag) << 16;
        move |= static_cast<Move_T>(promotion) << 19;
        return move;
    }

    void Move::print_uci() const {
        std::cout << square_str[static_cast<int>(get_from())];
        std::cout << square_str[static_cast<int>(get_to())];
        if (is_promotion()) {
            Promotion promotion = get_promotion();
            switch (promotion) {
                case Promotion::QUEEN:
                    std::cout << "q";
                    break;
                case Promotion::ROOK:
                    std::cout << "r";
                    break;
                case Promotion::KNIGHT:
                    std::cout << "n";
                    break;
                case Promotion::BISHOP:
                    std::cout << "b";
                    break;
                default:
                    break;
            }
        }
    }
}