#pragma once

#include <array>
#include <vector>
#include <string>

#include "../types.h"
#include "../defs.h"
#include "../move.h"
#include "../attacks/attacks.h"
#include "../utils/state.h"
#include "../utils/bits.h"
#include "../utils/static_vector.h"

namespace elixir {
    extern const std::string square_str[64];
    void print_square(Square sq);
    
    class Board {
    public:
        Board() {
            clear_board();
        }
        ~Board() = default;

        [[nodiscard]] inline PieceType piece_to_piecetype(Piece piece) { return static_cast<PieceType>(static_cast<int>(piece) / 2); }

        [[nodiscard]] inline Bitboard color_occupancy(Color color) const noexcept { return b_occupancies[static_cast<I8>(color)]; }
        [[nodiscard]] inline Bitboard color_occupancy(int color) const noexcept { return b_occupancies[color]; }

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

        template <Color C>
        [[nodiscard]] inline Bitboard pawns() const noexcept { return pawns() & color_occupancy(C); }
        template <Color C>
        [[nodiscard]] inline Bitboard knights() const noexcept { return knights() & color_occupancy(C); }
        template <Color C>
        [[nodiscard]] inline Bitboard bishops() const noexcept { return bishops() & color_occupancy(C); }
        template <Color C>
        [[nodiscard]] inline Bitboard rooks() const noexcept { return rooks() & color_occupancy(C); }
        template <Color C>
        [[nodiscard]] inline Bitboard queens() const noexcept { return queens() & color_occupancy(C); }
        template <Color C>
        [[nodiscard]] inline Bitboard king() const noexcept { return king() & color_occupancy(C); }

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

        [[nodiscard]] inline Bitboard minors() const noexcept { return knights() | bishops(); }

        [[nodiscard]] inline Bitboard black_minors() const noexcept { return minors() & black_occupancy(); }
        [[nodiscard]] inline Bitboard white_minors() const noexcept { return minors() & white_occupancy(); }

        [[nodiscard]] inline Bitboard majors() const noexcept { return rooks() | queens(); }

        [[nodiscard]] inline Bitboard black_majors() const noexcept { return majors() & black_occupancy(); }
        [[nodiscard]] inline Bitboard white_majors() const noexcept { return majors() & white_occupancy(); }

        [[nodiscard]] inline Bitboard all_pieces() const noexcept { return pawns() | knights() | bishops() | rooks() | queens() | king(); }
        [[nodiscard]] inline Bitboard all_pieces(Color color) const noexcept { return all_pieces() & color_occupancy(color); }

        [[nodiscard]] inline bool has_castling_rights(Color color) const noexcept { return castling_rights & (3 << 2*(static_cast<int>(color))); }

        [[nodiscard]] U64 get_board_hash();

        inline void set_piece(const Square sq, const PieceType piece, const Color color) {
            assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
            bits::set_bit(b_occupancies[static_cast<I8>(color)], sq);
            bits::set_bit(b_pieces[static_cast<I8>(piece)], sq);
        }

        inline void remove_piece(const Square sq, const PieceType piece, const Color color) {
            assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
            bits::clear_bit(b_occupancies[static_cast<I8>(color)], sq);
            bits::clear_bit(b_pieces[static_cast<I8>(piece)], sq);
        }

        [[nodiscard]] inline Piece piece_on(Square sq) const {
            
            assert(sq != Square::NO_SQ);
            
            for (int piece = 0; piece < 12; piece+=2) {
                U64 pieceMask = b_pieces[piece / 2];
                for (int color = 0; color < 2; color++) {
                    U64 occupancyMask = b_occupancies[color];
                    U64 combinedMask = pieceMask & occupancyMask;
                    if (bits::get_bit(combinedMask, sq)) {
                        return static_cast<Piece>(piece + color);
                    }
                }
            }
            return Piece::NO_PIECE;
        }

        [[nodiscard]] inline Color piece_color(Piece piece) const noexcept { return (static_cast<int>(piece) % 2 == 0) ? Color::WHITE : Color::BLACK; }

        constexpr void set_en_passant_square(Square sq) noexcept { en_passant_square = sq; }
        constexpr void set_side_to_move(Color color) noexcept { side = color; }
        constexpr void set_castling_rights(Castling rights) noexcept { castling_rights = rights; }
        constexpr void set_fifty_move_counter(I8 counter) noexcept { fifty_move_counter = counter; }
        constexpr void set_fullmove_number(I16 number) noexcept { fullmove_number = number; }
        constexpr void set_search_ply(I8 ply) noexcept { search_ply = ply; }

        inline void set_hash_key() noexcept { hash_key = get_board_hash(); }

        [[nodiscard]] inline Square get_en_passant_square() const noexcept { return en_passant_square; }
        [[nodiscard]] inline Color get_side_to_move() const noexcept { return side; }
        [[nodiscard]] inline Castling get_castling_rights() const noexcept { return castling_rights; }
        [[nodiscard]] inline I8 get_fifty_move_counter() const noexcept { return fifty_move_counter; }
        [[nodiscard]] inline I16 get_fullmove_number() const noexcept { return fullmove_number; }
        [[nodiscard]] inline I8 get_search_ply() const noexcept { return search_ply; }
        [[nodiscard]] inline U64 get_hash_key() const noexcept { return hash_key; }

        [[nodiscard]] inline Bitboard get_attackers(Square sq, Color c) const {
            Bitboard attackers = 0ULL;
            attackers |= (attacks::get_pawn_attacks(Color::WHITE, sq) & black_pawns());
            attackers |= (attacks::get_pawn_attacks(Color::BLACK, sq) & white_pawns());
            attackers |= (attacks::get_knight_attacks(sq) & knights());
            attackers |= (attacks::get_bishop_attacks(sq, occupancy()) & (bishops() | queens()));
            attackers |= (attacks::get_rook_attacks(sq, occupancy()) & (rooks() | queens()));
            attackers |= (attacks::get_king_attacks(sq) & king());
            return attackers & color_occupancy(c);
        }

        [[nodiscard]] inline bool is_square_attacked(Square sq, Color c) const {
            return (get_attackers(sq, c) != 0ULL) ? true : false;
        }

        void clear_board() noexcept;
        void from_fen(std::string fen);
        void to_startpos();

        void print_castling_rights() const noexcept;
        void print_board() const;

        bool make_move(move::Move move);
        void unmake_move(move::Move move, bool from_make_move);
        bool parse_uci_move(std::string move);
    private:
        std::array<Bitboard, 2> b_occupancies{};
        std::array<Bitboard, 6> b_pieces{};
        std::array<Square, 2> kings{};
        StaticVector<State, MAX_PLY> undo_stack;
        Square en_passant_square;
        Color side;
        Castling castling_rights;
        I8 fifty_move_counter;
        I16 fullmove_number;
        I8 search_ply;
        U64 hash_key;
    };
}