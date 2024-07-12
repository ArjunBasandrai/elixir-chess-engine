#pragma once

#include <array>
#include <string>
#include <vector>

#include "../attacks/attacks.h"
#include "../defs.h"
#include "../history.h"
#include "../move.h"
#include "../types.h"
#include "../utils/bits.h"
#include "../utils/state.h"
#include "../utils/static_vector.h"

namespace elixir {
    extern const std::string square_str[64];
    void print_square(const Square sq);

    class Board {
      public:
        Board() { clear_board(); }

        Board(std::string fen) { from_fen(fen); }

        ~Board() = default;

        [[nodiscard]] PieceType piece_to_piecetype(Piece piece) const {
            return static_cast<PieceType>(static_cast<int>(piece) / 2);
        }

        [[nodiscard]] Bitboard color_occupancy(Color color) const noexcept {
            return b_occupancies[static_cast<I8>(color)];
        }
        [[nodiscard]] Bitboard color_occupancy(int color) const noexcept {
            return b_occupancies[color];
        }

        [[nodiscard]] Bitboard piece_bitboard(PieceType piece) const noexcept {
            return b_pieces[static_cast<I8>(piece)];
        }

        [[nodiscard]] Bitboard occupancy() const noexcept {
            return b_occupancies[static_cast<I8>(Color::WHITE)] |
                   b_occupancies[static_cast<I8>(Color::BLACK)];
        }
        [[nodiscard]] Bitboard black_occupancy() const noexcept {
            return b_occupancies[static_cast<I8>(Color::BLACK)];
        }
        [[nodiscard]] Bitboard white_occupancy() const noexcept {
            return b_occupancies[static_cast<I8>(Color::WHITE)];
        }

        [[nodiscard]] Bitboard pawns() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::PAWN)];
        }
        [[nodiscard]] Bitboard knights() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::KNIGHT)];
        }
        [[nodiscard]] Bitboard bishops() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::BISHOP)];
        }
        [[nodiscard]] Bitboard rooks() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::ROOK)];
        }
        [[nodiscard]] Bitboard queens() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::QUEEN)];
        }
        [[nodiscard]] Bitboard king() const noexcept {
            return b_pieces[static_cast<I8>(PieceType::KING)];
        }

        template <Color C> [[nodiscard]] Bitboard pawns() const noexcept {
            return pawns() & color_occupancy(C);
        }
        template <Color C> [[nodiscard]] Bitboard knights() const noexcept {
            return knights() & color_occupancy(C);
        }
        template <Color C> [[nodiscard]] Bitboard bishops() const noexcept {
            return bishops() & color_occupancy(C);
        }
        template <Color C> [[nodiscard]] Bitboard rooks() const noexcept {
            return rooks() & color_occupancy(C);
        }
        template <Color C> [[nodiscard]] Bitboard queens() const noexcept {
            return queens() & color_occupancy(C);
        }
        template <Color C> [[nodiscard]] Bitboard king() const noexcept {
            return king() & color_occupancy(C);
        }

        [[nodiscard]] Bitboard black_pawns() const noexcept { return pawns() & black_occupancy(); }
        [[nodiscard]] Bitboard black_knights() const noexcept {
            return knights() & black_occupancy();
        }
        [[nodiscard]] Bitboard black_bishops() const noexcept {
            return bishops() & black_occupancy();
        }
        [[nodiscard]] Bitboard black_rooks() const noexcept { return rooks() & black_occupancy(); }
        [[nodiscard]] Bitboard black_queens() const noexcept {
            return queens() & black_occupancy();
        }
        [[nodiscard]] Bitboard black_king() const noexcept { return king() & black_occupancy(); }

        [[nodiscard]] Bitboard white_pawns() const noexcept { return pawns() & white_occupancy(); }
        [[nodiscard]] Bitboard white_knights() const noexcept {
            return knights() & white_occupancy();
        }
        [[nodiscard]] Bitboard white_bishops() const noexcept {
            return bishops() & white_occupancy();
        }
        [[nodiscard]] Bitboard white_rooks() const noexcept { return rooks() & white_occupancy(); }
        [[nodiscard]] Bitboard white_queens() const noexcept {
            return queens() & white_occupancy();
        }
        [[nodiscard]] Bitboard white_king() const noexcept { return king() & white_occupancy(); }

        [[nodiscard]] Bitboard minors() const noexcept { return knights() | bishops(); }

        [[nodiscard]] Bitboard black_minors() const noexcept {
            return minors() & black_occupancy();
        }
        [[nodiscard]] Bitboard white_minors() const noexcept {
            return minors() & white_occupancy();
        }

        [[nodiscard]] Bitboard majors() const noexcept { return rooks() | queens(); }

        [[nodiscard]] Bitboard black_majors() const noexcept {
            return majors() & black_occupancy();
        }
        [[nodiscard]] Bitboard white_majors() const noexcept {
            return majors() & white_occupancy();
        }

        [[nodiscard]] Bitboard all_pieces() const noexcept {
            return pawns() | knights() | bishops() | rooks() | queens() | king();
        }
        [[nodiscard]] Bitboard all_pieces(Color color) const noexcept {
            return all_pieces() & color_occupancy(color);
        }

        [[nodiscard]] bool has_castling_rights(Color color) const noexcept {
            return castling_rights & (3 << 2 * (static_cast<int>(color)));
        }

        [[nodiscard]] U64 get_board_hash();

        void set_piece(const Square sq, const PieceType piece, const Color color);
        void remove_piece(const Square sq, const PieceType piece, const Color color);

        [[nodiscard]] Piece piece_on(Square sq) const {
            assert(sq != Square::NO_SQ);
            return pieces[static_cast<I8>(sq)];
        }

        [[nodiscard]] Color piece_color(Piece piece) const noexcept {
            return (static_cast<int>(piece) % 2 == 0) ? Color::WHITE : Color::BLACK;
        }

        constexpr void set_en_passant_square(Square sq) noexcept { en_passant_square = sq; }
        constexpr void set_side_to_move(Color color) noexcept { side = color; }
        constexpr void set_castling_rights(Castling rights) noexcept { castling_rights = rights; }
        constexpr void set_fifty_move_counter(I8 counter) noexcept { fifty_move_counter = counter; }
        constexpr void set_fullmove_number(I16 number) noexcept { fullmove_number = number; }

        void set_hash_key() noexcept { hash_key = get_board_hash(); }

        [[nodiscard]] Square get_en_passant_square() const noexcept { return en_passant_square; }
        [[nodiscard]] Color get_side_to_move() const noexcept { return side; }
        [[nodiscard]] Castling get_castling_rights() const noexcept { return castling_rights; }
        [[nodiscard]] I8 get_fifty_move_counter() const noexcept { return fifty_move_counter; }
        [[nodiscard]] I16 get_fullmove_number() const noexcept { return fullmove_number; }
        [[nodiscard]] U64 get_hash_key() const noexcept { return hash_key; }
        [[nodiscard]] EvalScore get_eval() const noexcept { return eval; }

        [[nodiscard]] Square get_king_square(Color c) const noexcept {
            return kings[static_cast<I8>(c)];
        }

        [[nodiscard]] Bitboard get_attackers(Square sq, Color c, Bitboard occupancy) const {
            Bitboard attackers = 0ULL;
            attackers |= (attacks::get_pawn_attacks(Color::WHITE, sq) & black_pawns());
            attackers |= (attacks::get_pawn_attacks(Color::BLACK, sq) & white_pawns());
            attackers |= (attacks::get_knight_attacks(sq) & knights());
            attackers |= (attacks::get_bishop_attacks(sq, occupancy) & (bishops() | queens()));
            attackers |= (attacks::get_rook_attacks(sq, occupancy) & (rooks() | queens()));
            attackers |= (attacks::get_king_attacks(sq) & king());
            return attackers & color_occupancy(c);
        }

        [[nodiscard]] Bitboard get_attackers(Square sq, Color c) const {
            return get_attackers(sq, c, occupancy());
        }

        [[nodiscard]] bool is_square_attacked(Square sq, Color c) const {
            return (get_attackers(sq, c) != 0ULL) ? true : false;
        }

        [[nodiscard]] bool is_in_check() const {
            return is_square_attacked(kings[static_cast<I8>(side)],
                                      static_cast<Color>(static_cast<I8>(side) ^ 1));
        }

        void clear_board() noexcept;
        void clear_histories() noexcept {
            history.clear();
        }
        void from_fen(const std::string fen);
        void to_startpos();

        void print_castling_rights() const noexcept;
        void print_board() const;

        bool make_move(move::Move move);
        void unmake_move(const move::Move move, const bool from_make_move);
        void make_null_move();
        void unmake_null_move();

        move::Move parse_uci_move(const std::string move) const;
        bool play_uci_move(const std::string move);

        bool is_repetition() const;
        bool is_material_draw() const;

        int get_phase() const {
            int phase = bits::count_bits(minors()) + 2 * bits::count_bits(rooks()) +
                        4 * bits::count_bits(queens());
            phase = std::clamp(phase, 0, 24);
            return phase;
        }

        History history;

      private:
        std::array<Bitboard, 2> b_occupancies{};
        std::array<Bitboard, 6> b_pieces{};
        std::array<Square, 2> kings{};
        std::array<Piece, 64> pieces{};
        StaticVector<State, 1024> undo_stack;
        Square en_passant_square;
        Color side;
        Castling castling_rights;
        I8 fifty_move_counter;
        I16 fullmove_number;
        U64 hash_key;
        EvalScore eval;
    };
}