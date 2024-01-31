#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "board.h"

#include "../types.h"
#include "../defs.h"
#include "../utils/bits.h"
#include "../utils/str_utils.h"
#include "../utils/test_fens.h"
#include "../hashing/hash.h"

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
        bits::clear_bit(b_occupancies[static_cast<I8>(color)], sq);
        bits::clear_bit(b_pieces[static_cast<I8>(piece)], sq);
    }

    [[nodiscard]] Piece Board::piece_on(Square sq) {
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

    void Board::print_castling_rights() const noexcept {
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

        std::cout << "  Fifty Move counter: " << static_cast<int>(fifty_move_counter) << std::endl;
        std::cout << "  Fullmove number: " << static_cast<int>(fullmove_number) << std::endl;

        std::cout << "  Hash Key: " << hash_key << "ULL" << std::endl;

        std::cout << "  White King Square: ";
        print_square(static_cast<Square>(bits::lsb_index(white_king())));
        std::cout << std::endl;
        std::cout << "  Black King Square: ";
        print_square(static_cast<Square>(bits::lsb_index(black_king())));
        std::cout << std::endl << std::endl;
    }

    void Board::clear_board() noexcept {
        b_occupancies.fill(0ULL);
        b_pieces.fill(0ULL);
        kings.fill(Square::NO_SQ);
        en_passant_square = Square::NO_SQ;
        side = Color::WHITE;
        castling_rights = 0;
        fifty_move_counter = 0;
        fullmove_number = 0;
        search_ply = 0;
        hash_key = 0ULL;
    }

    void Board::from_fen(std::string fen) {
        clear_board();
        std::vector<std::string> params = str_utils::split(fen, ' ');
        
        const std::string position = params[0];
        const std::string move_right = params[1];
        const std::string castling = params[2];
        const std::string en_passant = params[3];

        fifty_move_counter = std::stoi(params.size() > 4 ? params[4] : "0");
        fullmove_number = std::stoi(params.size() > 5 ? params[5] : "1");

        std::vector<std::string> ranks = str_utils::split(position, '/');
        assert(ranks.size() == 8);
        for (int rank = 7; rank >= 0; rank--) {
            int file = 0;
            for (char c : ranks[7 - rank]) {
                if (isdigit(c)) {
                    file += c - '0';
                } else {
                    PieceType piece = static_cast<PieceType>(piece_str.find(c)/2);
                    assert(piece != PieceType::NO_PIECE_TYPE);
                    set_piece(static_cast<Square>(8 * rank + file), piece, isupper(c) ? Color::WHITE : Color::BLACK);
                    file++;
                }
            }
        }

        side = move_right == "w" ? Color::WHITE : Color::BLACK;

        castling_rights = 0;
        for (char c : castling) {
            switch (c) {
                case 'K':
                    castling_rights |= CASTLE_WHITE_KINGSIDE;
                    break;
                case 'Q':
                    castling_rights |= CASTLE_WHITE_QUEENSIDE;
                    break;
                case 'k':
                    castling_rights |= CASTLE_BLACK_KINGSIDE;
                    break;
                case 'q':
                    castling_rights |= CASTLE_BLACK_QUEENSIDE;
                    break;
                default:
                    break;
            }
        }

        en_passant_square = en_passant == "-" ? Square::NO_SQ : static_cast<Square>((en_passant[0] - 'a') + 8 * (en_passant[1] - '1'));
        search_ply = 0;

        kings[static_cast<I8>(Color::WHITE)] = static_cast<Square>(bits::lsb_index(white_king()));
        kings[static_cast<I8>(Color::BLACK)] = static_cast<Square>(bits::lsb_index(black_king()));

        set_hash_key();
    }

    void Board::to_startpos() {
        from_fen(start_position);
    }

    bool Board::make_move(move::Move move) {
        const Square from = move.get_from();
        const Square to = move.get_to();
        const Piece piece = move.get_piece();
        const move::Flag flag = move.get_flag();
        const move::Promotion promotion = move.get_promotion();

        const int stm = static_cast<int>(side);
        const Color enemy_side = static_cast<Color>(stm^1);
        const int xstm = static_cast<int>(enemy_side);

        const PieceType piecetype = piece_to_piecetype(piece);

        const int int_piece = static_cast<int>(piece);
        const int int_from = static_cast<int>(from);
        const int int_to = static_cast<int>(to);

        const Piece piece_ = piece_on(from);

        assert(from != Square::NO_SQ && to != Square::NO_SQ);
        assert(from != to);
        assert(piece != Piece::NO_PIECE);
        assert(piece_ == piece);
        assert(piece_color(piece_) == side);

        remove_piece(from, piecetype, side);
        set_piece(to, piecetype, side);

        hash_key ^= zobrist::piece_keys[int_piece][int_from];
        hash_key ^= zobrist::piece_keys[int_piece][int_to];

        fifty_move_counter++;
        if (side == Color::BLACK) {
            fullmove_number++;
        }

        if (piecetype == PieceType::PAWN) {
            fifty_move_counter = 0;
        }

        // Handling Captures
        if (flag == move::Flag::CAPTURE) {
            Piece captured_piece = piece_on(to);
            if (captured_piece != Piece::NO_PIECE) {
                fifty_move_counter = 0;
                remove_piece(to, piece_to_piecetype(captured_piece), enemy_side);
                hash_key ^= zobrist::piece_keys[static_cast<int>(captured_piece)][static_cast<int>(to)];
            }
        }

        // Handling Pawn Promotions
        if (flag == move::Flag::PROMOTION) {
            remove_piece(to, PieceType::PAWN, side);
            PieceType promotion_piece;
            switch (promotion) {
                case move::Promotion::QUEEN:
                    promotion_piece = PieceType::QUEEN;
                    break;
                case move::Promotion::ROOK:
                    promotion_piece = PieceType::ROOK;
                    break;
                case move::Promotion::KNIGHT:
                    promotion_piece = PieceType::KNIGHT;
                    break;
                case move::Promotion::BISHOP:
                    promotion_piece = PieceType::BISHOP;
                    break;
                default:
                    promotion_piece = PieceType::NO_PIECE_TYPE;
                    break;
            }
            assert(promotion_piece != PieceType::NO_PIECE_TYPE);
            set_piece(to, promotion_piece, side);
            hash_key ^= zobrist::piece_keys[int_piece][int_to];
            hash_key ^= zobrist::piece_keys[static_cast<int>(promotion_piece)+stm*6][int_to];
        }

        // Handling En Passant
        if (flag == move::Flag::EN_PASSANT) {
            Square captured_square = static_cast<Square>(int_to + (side == Color::WHITE ? -8 : 8));
            remove_piece(captured_square, PieceType::PAWN, enemy_side);
            hash_key ^= zobrist::piece_keys[static_cast<int>(PieceType::PAWN)+xstm*6][static_cast<int>(captured_square)];
        }
        if (en_passant_square != Square::NO_SQ) {
            hash_key ^= zobrist::ep_keys[static_cast<int>(en_passant_square)];
        }
        en_passant_square = Square::NO_SQ;

        // Handling Double Pawn Push
        if (flag == move::Flag::DOUBLE_PAWN_PUSH) {
            en_passant_square = static_cast<Square>(int_to + (side == Color::WHITE ? -8 : 8));
            hash_key ^= zobrist::ep_keys[static_cast<int>(en_passant_square)];
        }

        // Handling Castling
        if (flag == move::Flag::CASTLING) {
            int rook = static_cast<int>(PieceType::ROOK) + stm * 6;
            hash_key ^= zobrist::castle_keys[castling_rights];
            switch (to) {
                case Square::C1:
                    castling_rights &= ~CASTLE_WHITE_QUEENSIDE;
                    remove_piece(Square::A1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::D1, PieceType::ROOK, Color::WHITE);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::A1)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::D1)];
                    break;
                case Square::G1:
                    castling_rights &= ~CASTLE_WHITE_KINGSIDE;
                    remove_piece(Square::H1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::F1, PieceType::ROOK, Color::WHITE);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::H1)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::F1)];
                    break;
                case Square::C8:
                    castling_rights &= ~CASTLE_BLACK_QUEENSIDE;
                    remove_piece(Square::A8, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::D8, PieceType::ROOK, Color::BLACK);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::A8)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::D8)];
                    break;
                case Square::G8:
                    castling_rights &= ~CASTLE_BLACK_KINGSIDE;
                    remove_piece(Square::H1, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::F1, PieceType::ROOK, Color::BLACK);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::H1)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::F1)];
                    break;
                default:
                    break;
            }
            hash_key ^= zobrist::castle_keys[castling_rights];
        }   

        side = enemy_side;
        hash_key ^= zobrist::side_key;
        return true;     
    }
    
}