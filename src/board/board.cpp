#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "board.h"

#include "../defs.h"
#include "../evaluate.h"
#include "../hashing/hash.h"
#include "../texel/texel.h"
#include "../types.h"
#include "../utils/bits.h"
#include "../utils/eval_terms.h"
#include "../utils/state.h"
#include "../utils/str_utils.h"
#include "../utils/test_fens.h"

namespace elixir {
    const int castling_update[64] = {13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15,
                                     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                                     15, 15, 15, 15, 7,  15, 15, 15, 3,  15, 15, 11};

    const std::string square_str[64] = {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2",
                                        "c2", "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3",
                                        "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4",
                                        "g4", "h4", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
                                        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a7", "b7",
                                        "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8",
                                        "e8", "f8", "g8", "h8"};

    void print_square(const Square sq) {
        if (sq == Square::NO_SQ) {
            std::cout << "-";
            return;
        }
        std::cout << square_str[static_cast<I8>(sq)];
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

    void Board::print_board() const {
        std::cout << "  +---+----+----+---+" << std::endl;
        for (int rank = RANK_8; rank >= RANK_1; rank--) {
            std::cout << rank + 1 << " | ";
            for (int file = FILE_A; file <= FILE_H; file++) {
                Square sq   = static_cast<Square>(8 * rank + file);
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

        std::cout << "  Side to move: " << ((static_cast<int>(side) == 0) ? "White" : "Black")
                  << std::endl;

        std::cout << "  En passant square: ";
        print_square(en_passant_square);
        std::cout << std::endl;

        std::cout << "  Castling rights: ";
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
        pieces.fill(Piece::NO_PIECE);
        undo_stack.clear();
        en_passant_square  = Square::NO_SQ;
        side               = Color::WHITE;
        castling_rights    = 0;
        fifty_move_counter = 0;
        fullmove_number    = 0;
        hash_key           = 0ULL;
        eval               = 0;
    }

    void Board::set_piece(const Square sq, const PieceType piece, const Color color) {
        assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
        bits::set_bit(b_occupancies[static_cast<I8>(color)], sq);
        bits::set_bit(b_pieces[static_cast<I8>(piece)], sq);
        int square     = static_cast<I8>(sq);
        pieces[square] = static_cast<Piece>(static_cast<I8>(piece) * 2 + static_cast<I8>(color));
        if (color == Color::WHITE) {
            square ^= 56;
        }
        eval += (eval::material_score[static_cast<I8>(piece)] +
                 eval::psqt[static_cast<I8>(piece)][square]) *
                color_offset[static_cast<int>(color)];

        using namespace texel;
        TRACE_INCREMENT(material_score[static_cast<I8>(piece)], static_cast<I8>(color));
        TRACE_INCREMENT(psqt[static_cast<I8>(piece)][square], static_cast<I8>(color));
    }

    void Board::remove_piece(const Square sq, const PieceType piece, const Color color) {
        assert(sq != Square::NO_SQ && piece != PieceType::NO_PIECE_TYPE);
        bits::clear_bit(b_occupancies[static_cast<I8>(color)], sq);
        bits::clear_bit(b_pieces[static_cast<I8>(piece)], sq);
        int square = static_cast<int>(sq);
        assert(pieces[square] ==
               static_cast<Piece>(static_cast<I8>(piece) * 2 + static_cast<I8>(color)));
        pieces[square] = Piece::NO_PIECE;
        if (color == Color::WHITE) {
            square ^= 56;
        }
        eval -= (eval::material_score[static_cast<I8>(piece)] +
                 eval::psqt[static_cast<I8>(piece)][square]) *
                color_offset[static_cast<int>(color)];

        using namespace texel;
        TRACE_DECREMENT(material_score[static_cast<I8>(piece)], static_cast<I8>(color));
        TRACE_DECREMENT(psqt[static_cast<I8>(piece)][square], static_cast<I8>(color));
    }

    void Board::from_fen(const std::string fen) {
        clear_board();
        std::vector<std::string> params = str_utils::split(fen, ' ');

        const std::string position   = params[0];
        const std::string move_right = params[1];
        const std::string castling   = params[2];
        const std::string en_passant = params[3];

        fifty_move_counter = std::stoi(params.size() > 4 ? params[4] : "0");
        fullmove_number    = std::stoi(params.size() > 5 ? params[5] : "1");

        std::vector<std::string> ranks = str_utils::split(position, '/');

        assert(ranks.size() == 8);

        for (int rank = 7; rank >= 0; rank--) {
            int file = 0;
            for (char c : ranks[7 - rank]) {
                if (isdigit(c)) {
                    file += c - '0';
                } else {
                    const PieceType piece = static_cast<PieceType>(piece_str.find(c) / 2);

                    assert(piece != PieceType::NO_PIECE_TYPE);

                    set_piece(static_cast<Square>(8 * rank + file), piece,
                              isupper(c) ? Color::WHITE : Color::BLACK);
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

        en_passant_square =
            en_passant == "-"
                ? Square::NO_SQ
                : static_cast<Square>((en_passant[0] - 'a') + 8 * (en_passant[1] - '1'));

        kings[static_cast<I8>(Color::WHITE)] = static_cast<Square>(bits::lsb_index(white_king()));
        kings[static_cast<I8>(Color::BLACK)] = static_cast<Square>(bits::lsb_index(black_king()));

        set_hash_key();
    }

    void Board::to_startpos() {
        from_fen(start_position);
    }

    void Board::unmake_move(const move::Move move, const bool from_make_move) {
        const Square from     = move.get_from();
        const Square to       = move.get_to();
        const Piece piece     = move.get_piece();
        const move::Flag flag = move.get_flag();

        const PieceType piecetype = piece_to_piecetype(piece);
        int stm                   = static_cast<int>(side);
        Color enemy_side          = static_cast<Color>(stm ^ 1);
        int xstm                  = static_cast<int>(enemy_side);

        if (from_make_move) {
            std::swap(side, enemy_side);
            std::swap(stm, xstm);
        }

        // Handling Pawn Promotion
        if (move.is_promotion()) {
            switch (move.get_promotion()) {
                case move::Promotion::QUEEN:
                    remove_piece(to, PieceType::QUEEN, side);
                    break;
                case move::Promotion::ROOK:
                    remove_piece(to, PieceType::ROOK, side);
                    break;
                case move::Promotion::KNIGHT:
                    remove_piece(to, PieceType::KNIGHT, side);
                    break;
                case move::Promotion::BISHOP:
                    remove_piece(to, PieceType::BISHOP, side);
                    break;
                default:
                    break;
            }
        } else {
            remove_piece(to, piecetype, side);
        }

        set_piece(from, piecetype, side);

        if (piece == Piece::wK || piece == Piece::bK) {
            kings[static_cast<I8>(side)] = from;
        }

        State s              = undo_stack[undo_stack.size() - 1];
        hash_key             = s.hash_key;
        castling_rights      = s.castling_rights;
        en_passant_square    = s.enpass;
        fifty_move_counter   = s.fifty_move_counter;
        Piece captured_piece = s.captured_piece;

        if (side == Color::BLACK) {
            fullmove_number--;
        }

        // Handling Captures
        if (move.is_capture() && captured_piece != Piece::NO_PIECE) {
            set_piece(to, piece_to_piecetype(captured_piece), enemy_side);
        }

        // Handling En Passant
        if (flag == move::Flag::EN_PASSANT) {
            const int int_to       = static_cast<int>(to);
            Square captured_square = static_cast<Square>(int_to + (side == Color::WHITE ? -8 : 8));
            set_piece(captured_square, PieceType::PAWN, enemy_side);
        }

        // Handling Castling
        if (flag == move::Flag::CASTLING) {
            switch (to) {
                case Square::C1:
                    remove_piece(Square::D1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::A1, PieceType::ROOK, Color::WHITE);
                    break;
                case Square::G1:
                    remove_piece(Square::F1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::H1, PieceType::ROOK, Color::WHITE);
                    break;
                case Square::C8:
                    remove_piece(Square::D8, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::A8, PieceType::ROOK, Color::BLACK);
                    break;
                case Square::G8:
                    remove_piece(Square::F8, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::H8, PieceType::ROOK, Color::BLACK);
                    break;
                default:
                    break;
            }
        }

        undo_stack.pop_back();

        return;
    }

    bool Board::make_move(move::Move move) {
        const Square from               = move.get_from();
        const Square to                 = move.get_to();
        const Piece piece               = move.get_piece();
        const move::Flag flag           = move.get_flag();
        const move::Promotion promotion = move.get_promotion();

        const int stm          = static_cast<int>(side);
        const Color enemy_side = static_cast<Color>(stm ^ 1);
        const int xstm         = static_cast<int>(enemy_side);

        const PieceType piecetype = piece_to_piecetype(piece);

        const int int_piece = static_cast<int>(piece);
        const int int_from  = static_cast<int>(from);
        const int int_to    = static_cast<int>(to);

        const Piece piece_ = piece_on(from);

        assert(from != Square::NO_SQ && to != Square::NO_SQ);
        assert(from != to);
        assert(piece != Piece::NO_PIECE);
        assert(piece_ == piece);
        assert(piece_color(piece_) == side);

        Piece captured_piece = piece_on(to);
        State s = State(hash_key, castling_rights, en_passant_square, fifty_move_counter,
                        captured_piece, eval);
        undo_stack.push(s);

        eval = s.eval;

        remove_piece(from, piecetype, side);
        // Move source piece to target only if not a capturing move
        // In case of a capture, moving of piece is handled in the "Handling Captures" section
        if (! move.is_capture()) {

            assert(captured_piece == Piece::NO_PIECE);

            set_piece(to, piecetype, side);
        }

        if (piece_ == Piece::wK || piece_ == Piece::bK) {
            kings[static_cast<I8>(side)] = to;
        }

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
        if (move.is_capture()) {
            if (captured_piece != Piece::NO_PIECE) {
                fifty_move_counter = 0;
                remove_piece(to, piece_to_piecetype(captured_piece), enemy_side);
                set_piece(to, piecetype, side);
                hash_key ^=
                    zobrist::piece_keys[static_cast<int>(captured_piece)][static_cast<int>(to)];
            }
        }

        // Handling Pawn Promotions
        if (move.is_promotion()) {
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
            hash_key ^= zobrist::piece_keys[static_cast<int>(promotion_piece) + stm * 6][int_to];
        }

        // Handling En Passant
        if (flag == move::Flag::EN_PASSANT && en_passant_square != Square::NO_SQ) {
            Square captured_square = static_cast<Square>(int_to - 8 * color_offset[stm]);
            remove_piece(captured_square, PieceType::PAWN, enemy_side);
            hash_key ^= zobrist::piece_keys[static_cast<int>(PieceType::PAWN) + xstm * 6]
                                           [static_cast<int>(captured_square)];
        }
        if (en_passant_square != Square::NO_SQ) {
            hash_key ^= zobrist::ep_keys[static_cast<int>(en_passant_square)];
        }
        en_passant_square = Square::NO_SQ;

        // Handling Double Pawn Push
        if (flag == move::Flag::DOUBLE_PAWN_PUSH) {
            en_passant_square = static_cast<Square>(int_to - 8 * color_offset[stm]);
            hash_key ^= zobrist::ep_keys[static_cast<int>(en_passant_square)];
        }

        // Handling Castling
        if (flag == move::Flag::CASTLING) {
            int rook = static_cast<int>(PieceType::ROOK) + stm * 6;
            switch (to) {
                case Square::C1:
                    remove_piece(Square::A1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::D1, PieceType::ROOK, Color::WHITE);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::A1)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::D1)];
                    break;
                case Square::G1:
                    remove_piece(Square::H1, PieceType::ROOK, Color::WHITE);
                    set_piece(Square::F1, PieceType::ROOK, Color::WHITE);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::H1)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::F1)];
                    break;
                case Square::C8:
                    remove_piece(Square::A8, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::D8, PieceType::ROOK, Color::BLACK);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::A8)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::D8)];
                    break;
                case Square::G8:
                    remove_piece(Square::H8, PieceType::ROOK, Color::BLACK);
                    set_piece(Square::F8, PieceType::ROOK, Color::BLACK);
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::H8)];
                    hash_key ^= zobrist::piece_keys[rook][static_cast<int>(Square::F8)];
                    break;
                default:
                    break;
            }
        }

        if (is_square_attacked(kings[static_cast<I8>(side)], enemy_side)) {
            unmake_move(move, false);
            return false;
        }

        side = enemy_side;
        hash_key ^= zobrist::castle_keys[castling_rights];
        castling_rights &= castling_update[int_from];
        castling_rights &= castling_update[int_to];
        hash_key ^= zobrist::castle_keys[castling_rights];

        hash_key ^= zobrist::side_key;
        return true;
    }

    void Board::make_null_move() {
        const State s = State(hash_key, castling_rights, en_passant_square, fifty_move_counter,
                              Piece::NO_PIECE, eval);
        undo_stack.push(s);
        fifty_move_counter++;
        if (en_passant_square != Square::NO_SQ) {
            hash_key ^= zobrist::ep_keys[static_cast<int>(en_passant_square)];
        }
        en_passant_square = Square::NO_SQ;
        hash_key ^= zobrist::side_key;
        side = static_cast<Color>(static_cast<int>(side) ^ 1);
    }

    void Board::unmake_null_move() {
        const State s = undo_stack[undo_stack.size() - 1];
        undo_stack.pop_back();
        hash_key           = s.hash_key;
        fifty_move_counter = s.fifty_move_counter;
        en_passant_square  = s.enpass;
        castling_rights    = s.castling_rights;
        eval               = s.eval;
        side               = static_cast<Color>(static_cast<int>(side) ^ 1);
    }

    move::Move Board::parse_uci_move(const std::string move) const {

        assert(move.length() == 4 || move.length() == 5);

        Square from               = static_cast<Square>((move[0] - 'a') + 8 * (move[1] - '1'));
        Square to                 = static_cast<Square>((move[2] - 'a') + 8 * (move[3] - '1'));
        Piece piece               = piece_on(from);
        move::Flag flag           = move::Flag::NORMAL;
        move::Promotion promotion = move::Promotion::QUEEN;
        if (move.length() == 5) {
            switch (move[4]) {
                case 'q':
                    promotion = move::Promotion::QUEEN;
                    break;
                case 'r':
                    promotion = move::Promotion::ROOK;
                    break;
                case 'n':
                    promotion = move::Promotion::KNIGHT;
                    break;
                case 'b':
                    promotion = move::Promotion::BISHOP;
                    break;
                default:
                    break;
            }
            flag = move::Flag::PROMOTION;
        }

        if (piece_on(to) != Piece::NO_PIECE) {
            if (get_rank(to) == PromotionRank[static_cast<I8>(side)] &&
                flag == move::Flag::PROMOTION) {
                flag = move::Flag::CAPTURE_PROMOTION;
            } else {
                flag = move::Flag::CAPTURE;
            }
        }

        if (piece == Piece::wK && from == Square::E1 && to == Square::G1) {
            flag = move::Flag::CASTLING;
        } else if (piece == Piece::wK && from == Square::E1 && to == Square::C1) {
            flag = move::Flag::CASTLING;
        } else if (piece == Piece::bK && from == Square::E8 && to == Square::G8) {
            flag = move::Flag::CASTLING;
        } else if (piece == Piece::bK && from == Square::E8 && to == Square::C8) {
            flag = move::Flag::CASTLING;
        }

        int to_rank   = get_rank(to);
        int from_rank = get_rank(from);

        if (piece == Piece::wP && from_rank == RANK_2 && to_rank == RANK_4) {
            flag = move::Flag::DOUBLE_PAWN_PUSH;
        } else if (piece == Piece::bP && from_rank == RANK_7 && to_rank == RANK_5) {
            flag = move::Flag::DOUBLE_PAWN_PUSH;
        }

        if (en_passant_square == to && (piece == Piece::wP || piece == Piece::bP)) {
            flag = move::Flag::EN_PASSANT;
        }

        move::Move m;
        m.set_move(from, to, piece, flag, promotion);
        return m;
    }

    bool Board::play_uci_move(const std::string move) {
        move::Move m = parse_uci_move(move);
        return make_move(m);
    }

    bool Board::is_repetition() const {
        const auto limit = std::max<int>(0, undo_stack.size() - fifty_move_counter - 2);
        int counter      = 1;
        for (int i = undo_stack.size() - 4; i >= limit; i -= 2) {
            if (undo_stack[i].hash_key == hash_key) {
                if (--counter == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Board::is_material_draw() const {
        using namespace bits;
        const int board_piece_count = count_bits(occupancy());
        if (board_piece_count == 2) {
            return true;
        } else if (board_piece_count == 3 && (count_bits(minors()) == 1)) {
            return true;
        } else if (board_piece_count == 4) {
            if (count_bits(knights()) == 2) {
                return true;
            } else if (count_bits(white_bishops()) == 1 && count_bits(black_bishops()) == 1) {
                return true;
            }
        }
        return false;
    }
}