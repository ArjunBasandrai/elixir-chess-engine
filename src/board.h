#pragma once

#include <array>

#include "types.h"
#include "defs.h"

namespace elixir {
    class Board {
    public:
        Board() {
            for (int i=0; i<2; i++) {
                b_occupancies[i] = 0ULL;
            }
            for (int i=0; i<6; i++) {
                b_pieces[i] = 0ULL;
            }
            en_passant_square = Square::NO_SQ;
            side_to_move = Color::WHITE;
            castling_rights = 0;
        }
        ~Board() = default;
        
        [[nodiscard]] inline Bitboard color_occupancy(Color color) const noexcept { return b_occupancies[static_cast<I8>(color)]; }

        [[nodiscard]] inline Bitboard piece_bitboard(PieceType piece) const noexcept { return b_pieces[static_cast<I8>(piece)]; }

        [[nodiscard]] inline Bitboard occupancy() const noexcept { return b_occupancies[static_cast<I8>(Color::WHITE)] | b_occupancies[static_cast<I8>(Color::BLACK)]; }
        [[nodiscard]] inline Bitboard black_occupancy() const noexcept { return b_occupancies[static_cast<I8>(Color::BLACK)]; }
        [[nodiscard]] inline Bitboard white_occupancy() const noexcept { return b_occupancies[static_cast<I8>(Color::WHITE)]; }

        [[nodiscard]] inline Bitboard pawns() const noexcept { return b_pieces[static_cast<I8>(PieceType::PAWN)]; }
        [[nodiscard]] inline Bitboard knights() const noexcept { return b_pieces[static_cast<I8>(PieceType::KNIGHT)]; }
        [[nodiscard]] inline Bitboard bishops() const noexcept { return b_pieces[static_cast<I8>(PieceType::BISHOP)]; }
        [[nodiscard]] inline Bitboard rooks() const noexcept { return b_pieces[static_cast<I8>(PieceType::ROOK)]; }
        [[nodiscard]] inline Bitboard queens() const noexcept { return b_pieces[static_cast<I8>(PieceType::QUEEN)]; }
        [[nodiscard]] inline Bitboard king() const noexcept { return b_pieces[static_cast<I8>(PieceType::KING)]; }

        [[nodiscard]] inline Bitboard black_pawns() const noexcept { return pawns() & black_occupancy(); }
        [[nodiscard]] inline Bitboard black_knights() const noexcept { return knights() & black_occupancy(); }
        [[nodiscard]] inline Bitboard black_bishops() const noexcept { return bishops() & black_occupancy(); }
        [[nodiscard]] inline Bitboard black_rooks() const noexcept { return rooks() & black_occupancy(); }
        [[nodiscard]] inline Bitboard black_queens() const noexcept { return queens() & black_occupancy(); }
        [[nodiscard]] inline Bitboard black_king() const noexcept { return king() & black_occupancy(); }

        [[nodiscard]] inline Bitboard white_pawns() const noexcept { return pawns() & white_occupancy(); }
        [[nodiscard]] inline Bitboard white_knights() const noexcept { return knights() & white_occupancy(); }
        [[nodiscard]] inline Bitboard white_bishops() const noexcept { return bishops() & white_occupancy(); }
        [[nodiscard]] inline Bitboard white_rooks() const noexcept { return rooks() & white_occupancy(); }
        [[nodiscard]] inline Bitboard white_queens() const noexcept { return queens() & white_occupancy(); }
        [[nodiscard]] inline Bitboard white_king() const noexcept { return king() & white_occupancy(); }

        [[nodiscard]] inline Bitboard minors() { return knights() | bishops(); }

        [[nodiscard]] inline Bitboard black_minors() { return minors() & black_occupancy(); }
        [[nodiscard]] inline Bitboard white_minors() { return minors() & white_occupancy(); }

        [[nodiscard]] inline Bitboard majors() { return rooks() | queens(); }

        [[nodiscard]] inline Bitboard black_majors() { return majors() & black_occupancy(); }
        [[nodiscard]] inline Bitboard white_majors() { return majors() & white_occupancy(); }

        [[nodiscard]] inline Bitboard all_pieces() const noexcept { return pawns() | knights() | bishops() | rooks() | queens() | king(); }
        [[nodiscard]] inline Bitboard all_pieces(Color color) const noexcept { return all_pieces() & color_occupancy(color); }

        [[nodiscard]] inline bool has_castling_rights(Color color) { return castling_rights & (3 << 2*(static_cast<int>(color))); }

        void set_piece(Square sq, PieceType piece, Color color);
    private:
        std::array<Bitboard, 2> b_occupancies{};
        std::array<Bitboard, 6> b_pieces{};
        Square en_passant_square;
        Color side_to_move;
        Castling castling_rights;
        I8 halfmove_clock;
        I16 fullmove_number;
        I8 search_ply;
    };
}