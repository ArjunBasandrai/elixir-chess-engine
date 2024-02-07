#pragma once

#include "types.h"
#include "defs.h"

namespace elixir::move {
    enum class Flag {
        NORMAL,
        CAPTURE,
        DOUBLE_PAWN_PUSH,
        EN_PASSANT,
        CASTLING,
        PROMOTION
    };
    enum class Promotion {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
    };
    class Move {
    public:
        [[nodiscard]] Move_T encode_move(Square from, Square to, Piece piece, Flag flag, Promotion promotion);
        [[nodiscard]] Move_T encode_move(Square from, Square to, int piece, Flag flag, Promotion promotion);
        [[nodiscard]] Move_T encode_move(int from, int to, Piece piece, Flag flag, Promotion promotion);
        [[nodiscard]] Move_T encode_move(int from, int to, int piece, Flag flag, Promotion promotion);

        [[nodiscard]] constexpr Square get_from() const { return static_cast<Square>(m_move & 0x3f); }
        [[nodiscard]] constexpr Square get_to() const { return static_cast<Square>((m_move >> 6) & 0x3f); }
        [[nodiscard]] constexpr Piece get_piece() const { return static_cast<Piece>((m_move >> 12) & 0xf); }
        [[nodiscard]] constexpr Flag get_flag() const { return static_cast<Flag>((m_move >> 16) & 0x7); }
        [[nodiscard]] constexpr Promotion get_promotion() const { return static_cast<Promotion>((m_move >> 19) & 0x3); }
        [[nodiscard]] constexpr Move_T get_move() const { return m_move; }

        void constexpr set_move(Move_T move) { m_move = move; }
        void constexpr set_move(Square from, Square to, Piece piece, Flag flag, Promotion promotion) { m_move = encode_move(from, to, piece, flag, promotion); }
        void constexpr set_move(Square from, Square to, int piece, Flag flag, Promotion promotion) { m_move = encode_move(from, to, piece, flag, promotion); }
        void constexpr set_move(int from, int to, Piece piece, Flag flag, Promotion promotion) { m_move = encode_move(from, to, piece, flag, promotion); }
        void constexpr set_move(int from, int to, int piece, Flag flag, Promotion promotion) { m_move = encode_move(from, to, piece, flag, promotion); }
        
        [[nodiscard]] constexpr bool is_capture() const { return get_flag() == Flag::CAPTURE; }
        [[nodiscard]] constexpr bool is_double_pawn_push() const { return get_flag() == Flag::DOUBLE_PAWN_PUSH; }
        [[nodiscard]] constexpr bool is_en_passant() const { return get_flag() == Flag::EN_PASSANT; }
        [[nodiscard]] constexpr bool is_castling() const { return get_flag() == Flag::CASTLING; }
        [[nodiscard]] constexpr bool is_promotion() const { return get_flag() == Flag::PROMOTION; }
        [[nodiscard]] constexpr bool is_normal() const { return get_flag() == Flag::NORMAL; }
        [[nodiscard]] constexpr bool is_quiet() const { return !(is_capture() || is_en_passant()); }

        void print_uci();
    private:
        Move_T m_move;
    };
}   