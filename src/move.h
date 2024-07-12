#pragma once

#include "defs.h"
#include "types.h"
#include "utils/static_vector.h"

namespace elixir::move {
    enum class Flag {
        NORMAL,
        CAPTURE,
        DOUBLE_PAWN_PUSH,
        EN_PASSANT,
        CASTLING,
        PROMOTION,
        CAPTURE_PROMOTION
    };
    enum class Promotion {
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
    };
    class Move {
      public:
        Move() = default;
        Move(Move_T move);
        Move(Square from, Square to, Piece piece, Flag flag, Promotion promotion);
        ~Move() = default;

        [[nodiscard]] Move_T encode_move(Square from, Square to, Piece piece, Flag flag,
                                         Promotion promotion) const noexcept;

        [[nodiscard]] constexpr Square get_from() const noexcept {
            return static_cast<Square>(m_move & 0x3f);
        }
        [[nodiscard]] constexpr Square get_to() const noexcept {
            return static_cast<Square>((m_move >> 6) & 0x3f);
        }
        [[nodiscard]] constexpr Piece get_piece() const noexcept {
            return static_cast<Piece>((m_move >> 12) & 0xf);
        }
        [[nodiscard]] constexpr Flag get_flag() const noexcept {
            return static_cast<Flag>((m_move >> 16) & 0x7);
        }
        [[nodiscard]] constexpr Promotion get_promotion() const noexcept {
            return static_cast<Promotion>((m_move >> 19) & 0x3);
        }
        [[nodiscard]] constexpr Move_T get_move() const noexcept { return m_move; }

        void set_move(Move_T move) noexcept { m_move = move; }
        void set_move(Square from, Square to, Piece piece, Flag flag,
                      Promotion promotion) noexcept {
            m_move = encode_move(from, to, piece, flag, promotion);
        }

        [[nodiscard]] constexpr bool is_capture() const noexcept {
            Flag f = get_flag();
            return f == Flag::CAPTURE || f == Flag::CAPTURE_PROMOTION;
        }
        [[nodiscard]] constexpr bool is_double_pawn_push() const noexcept {
            return get_flag() == Flag::DOUBLE_PAWN_PUSH;
        }
        [[nodiscard]] constexpr bool is_en_passant() const noexcept {
            return get_flag() == Flag::EN_PASSANT;
        }
        [[nodiscard]] constexpr bool is_castling() const noexcept {
            return get_flag() == Flag::CASTLING;
        }
        [[nodiscard]] constexpr bool is_promotion() const noexcept {
            return get_flag() == Flag::PROMOTION || get_flag() == Flag::CAPTURE_PROMOTION;
        }
        [[nodiscard]] constexpr bool is_normal() const noexcept {
            return get_flag() == Flag::NORMAL;
        }
        [[nodiscard]] constexpr bool is_quiet() const noexcept {
            return ! (is_capture() || is_en_passant());
        }

        bool operator==(const Move &other) const noexcept { return m_move == other.m_move; }

        explicit operator bool() const noexcept { return m_move != 0; }

        void print_uci() const;

      private:
        Move_T m_move;
    };
    const Move NO_MOVE = Move(0);
}

namespace elixir {
    using MoveList = StaticVector<move::Move, 256>;
}