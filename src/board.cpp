#include <iostream>
#include <cassert>

#include "board.h"

#include "types.h"
#include "defs.h"
#include "bits.h"

namespace elixir {
    const std::string square_str[64] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
    };

    void print_square(Square sq) {
        if (sq == Square::NO_SQ) {
            std::cout << "-";
            return;
        }
        std::cout << square_str[static_cast<I8>(sq)];
    }

    void Board::set_piece(Square sq, PieceType piece, Color color) {
        assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
        bits::set_bit(b_occupancies[static_cast<I8>(color)], sq);
        bits::set_bit(b_pieces[static_cast<I8>(piece)], sq);
    }

    void Board::remove_piece(Square sq, PieceType piece, Color color) {
        assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
        bits::set_bit(b_occupancies[static_cast<I8>(color)], sq);
        bits::set_bit(b_pieces[static_cast<I8>(piece)], sq);
    }

    Piece Board::piece_on(Square sq) {
        assert(sq != Square::NO_SQ);
        for (int piece = 0; piece < 12; piece++) {
            for (int color = 0; color < 2; color++) {
                if (bits::get_bit(b_pieces[piece/2] & b_occupancies[color], sq)) {
                    return static_cast<Piece>(piece+color);
                }
            }
        }
        return Piece::NO_PIECE;
    }

    void Board::print_castling_rights() {
        if (castling_rights == 0) {
            std::cout << "-";
        } else {
            if (castling_rights & CASTLE_WHITE_KINGSIDE) {
                std::cout << "K";
            }
            if (castling_rights & CASTLE_WHITE_QUEENSIDE) {
                std::cout << "Q";
            }
            if (castling_rights & CASTLE_BLACK_KINGSIDE) {
                std::cout << "k";
            }
            if (castling_rights & CASTLE_BLACK_QUEENSIDE) {
                std::cout << "q";
            }
        }
    }
    
    void Board::print_board() {
        std::cout << "  +---+----+----+---+" << std::endl;
        for (int rank = RANK_8; rank >= RANK_1; rank--) {
            std::cout << rank + 1 << " | ";
            for (int file = FILE_A; file <= FILE_H; file++) {
                Square sq = static_cast<Square>(8 * rank + file);
                Piece piece = piece_on(sq);
                if (piece == Piece::NO_PIECE) {
                    std::cout << ". ";
                } else {
                    std::cout << piece_str[static_cast<I8>(piece)] << " ";
                }
            }
            std::cout << "|" << std::endl;
        }
        std::cout << "  +---+----+----+---+" << std::endl;
        std::cout << "    a b c d e f g h" << std::endl;

        std::cout << std::endl;

        std::cout << "  Side to move: " << ((static_cast<int>(side)==0) ? "White" : "Black") << std::endl;

        std::cout << "  En passant square: "; 
        print_square(en_passant_square); 
        std::cout << std::endl;
        
        std::cout << "  Castling rights: " ;
        print_castling_rights();
        std::cout << std::endl;

        std::cout << "  Fifty Move counter: " << static_cast<int>(halfmove_clock) << std::endl;
        std::cout << "  Fullmove number: " << static_cast<int>(fullmove_number) << std::endl;
    }

}