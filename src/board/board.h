#pragma once

#include <array>
#include <string>

#include "../types.h"
#include "../defs.h"

namespace elixir {
    extern const std::string square_str[64];
    void print_square(Square sq);
    class Board {
    public:
        Board() {
            clear_board();
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
        void remove_piece(Square sq, PieceType piece, Color color);
        Piece piece_on(Square sq);

        void set_en_passant_square(Square sq) { en_passant_square = sq; }
        void set_side_to_move(Color color) { side = color; }
        void set_castling_rights(Castling rights) { castling_rights = rights; }
        void set_halfmove_clock(I8 clock) { halfmove_clock = clock; }
        void set_fullmove_number(I16 number) { fullmove_number = number; }
        void set_search_ply(I8 ply) { search_ply = ply; }

        [[nodiscard]] inline Square get_en_passant_square() const noexcept { return en_passant_square; }
        [[nodiscard]] inline Color get_side_to_move() const noexcept { return side; }
        [[nodiscard]] inline Castling get_castling_rights() const noexcept { return castling_rights; }
        [[nodiscard]] inline I8 get_halfmove_clock() const noexcept { return halfmove_clock; }
        [[nodiscard]] inline I16 get_fullmove_number() const noexcept { return fullmove_number; }
        [[nodiscard]] inline I8 get_search_ply() const noexcept { return search_ply; }

        void clear_board();
        void from_fen(std::string fen);
        void to_startpos();

        void print_castling_rights();
        void print_board();
    private:
        std::array<Bitboard, 2> b_occupancies{};
        std::array<Bitboard, 6> b_pieces{};
        std::array<Square, 2> kings{};
        Square en_passant_square;
        Color side;
        Castling castling_rights;
        I8 halfmove_clock;
        I16 fullmove_number;
        I8 search_ply;
    };
}