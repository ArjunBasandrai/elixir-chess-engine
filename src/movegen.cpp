#include <iostream>
#include <vector>
#include <cassert>

#include "movegen.h"

#include "types.h"
#include "defs.h"
#include "utils/bits.h"

namespace elixir::movegen {
    void generate_quiet_pawn_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard pawns;
        I8 stm = static_cast<int>(side);
        Piece piece;
        
        switch (side) {
            case Color::WHITE:
                pawns = board.pawns<Color::WHITE>();
                piece = Piece::wP;
                break;
            case Color::BLACK:
                pawns = board.pawns<Color::BLACK>();
                piece = Piece::bP;
                break;
            default:
                assert(false);
                break;
        }
        
        while (pawns) {
            move::Move m;
            Square source = static_cast<Square>(bits::pop_bit(pawns));
            Square target = static_cast<Square>(static_cast<int>(source) + 8 * color_offset[stm]);
            int target_rank = get_rank(target);
            int source_rank = get_rank(source);
            
            // Quiet move check
            if (board.piece_on(target) != Piece::NO_PIECE) {
                continue;
            }
            
            if (target_rank == PromotionRank[stm]) { 
                m.set_move(source, target, piece, move::Flag::PROMOTION, move::Promotion::QUEEN);
                moves.push_back(m);
                m.set_move(source, target, piece, move::Flag::PROMOTION, move::Promotion::ROOK);
                moves.push_back(m);
                m.set_move(source, target, piece, move::Flag::PROMOTION, move::Promotion::BISHOP);
                moves.push_back(m);
                m.set_move(source, target, piece, move::Flag::PROMOTION, move::Promotion::KNIGHT);
                moves.push_back(m);
            } else {
                m.set_move(source, target, piece, move::Flag::NORMAL, move::Promotion::QUEEN);
                moves.push_back(m);
                target = static_cast<Square>(static_cast<int>(source) + 2 * 8 * color_offset[stm]);
                if (source_rank == DoublePawnRank[stm] && board.piece_on(target) == Piece::NO_PIECE){
                    m.set_move(source, target, piece, move::Flag::DOUBLE_PAWN_PUSH, move::Promotion::QUEEN);
                    moves.push_back(m);
                }
            }
        }
    }

    void generate_capture_pawn_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard pawns;
        I8 stm = static_cast<int>(side);
        I8 xstm = stm^1;
        Piece piece;

        switch (side) {
            case Color::WHITE:
                pawns = board.pawns<Color::WHITE>();
                piece = Piece::wP;
                break;
            case Color::BLACK:
                pawns = board.pawns<Color::BLACK>();
                piece = Piece::bP;
                break;
            default:
                assert(false);
                break;
        }

        while (pawns) {
            move::Move m;
            Square source = static_cast<Square>(bits::pop_bit(pawns));
            Bitboard attacks = attacks::get_pawn_attacks(side, source) & board.color_occupancy(xstm);

            while (attacks) {
                Square target = static_cast<Square>(bits::pop_bit(attacks));
                assert(board.piece_on(target) != Piece::NO_PIECE);
                int target_rank = get_rank(target);
                int source_rank = get_rank(source);
                if (target_rank == PromotionRank[stm]) { 
                    m.set_move(source, target, piece, move::Flag::CAPTURE_PROMOTION, move::Promotion::QUEEN);
                    moves.push_back(m);
                    m.set_move(source, target, piece, move::Flag::CAPTURE_PROMOTION, move::Promotion::ROOK);
                    moves.push_back(m);
                    m.set_move(source, target, piece, move::Flag::CAPTURE_PROMOTION, move::Promotion::BISHOP);
                    moves.push_back(m);
                    m.set_move(source, target, piece, move::Flag::CAPTURE_PROMOTION, move::Promotion::KNIGHT);
                    moves.push_back(m);
                } else {
                    m.set_move(source, target, piece, move::Flag::CAPTURE, move::Promotion::QUEEN);
                    moves.push_back(m);
                }
            }
        }
    }

    void generate_enpassant_pawn_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard pawns;
        Bitboard enemy_pawns;
        I8 stm = static_cast<int>(side);
        I8 xstm = stm^1;
        Color enemy_side = static_cast<Color>(xstm);
        Piece piece;

        switch (side) {
            case Color::WHITE:
                pawns = board.pawns<Color::WHITE>();
                enemy_pawns = board.pawns<Color::BLACK>();
                piece = Piece::wP;
                break;
            case Color::BLACK:
                pawns = board.pawns<Color::BLACK>();
                enemy_pawns = board.pawns<Color::WHITE>();
                piece = Piece::bP;
                break;
            default:
                assert(false);
                break;
        }
        Square ep_sq = board.get_en_passant_square();
        if (ep_sq != Square::NO_SQ) {
            Bitboard ep_attacks = attacks::get_pawn_attacks(enemy_side, ep_sq) & pawns;
            while (ep_attacks) {
                move::Move m;
                Square source = static_cast<Square>(bits::pop_bit(ep_attacks));
                m.set_move(source, ep_sq, piece, move::Flag::EN_PASSANT, move::Promotion::QUEEN);
                moves.push_back(m);
                
            }
        }
    }

    void generate_castling_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard king;
        Bitboard occupancy = board.occupancy();
        I8 stm = static_cast<int>(side);
        I8 xstm = stm^1;
        Color enemy_side = static_cast<Color>(xstm);
        Piece piece;
        Castling caslting_rights = board.get_castling_rights();

        switch (side) {
            case Color::WHITE:
                king = board.king<Color::WHITE>();
                piece = Piece::wK;
                if (caslting_rights & CASTLE_WHITE_KINGSIDE) {
                    if (!bits::get_bit(occupancy, Square::F1) && !bits::get_bit(occupancy, Square::G1)) {
                        if (!board.is_square_attacked(Square::E1, enemy_side) && !board.is_square_attacked(Square::F1, enemy_side)) {
                            move::Move m;
                            m.set_move(Square::E1, Square::G1, piece, move::Flag::CASTLING, move::Promotion::QUEEN);
                            moves.push_back(m);
                        }
                    }
                } else if (caslting_rights & CASTLE_WHITE_QUEENSIDE) {
                    if (!bits::get_bit(occupancy, Square::D1) && !bits::get_bit(occupancy, Square::C1) && !bits::get_bit(occupancy, Square::B1)) {
                        if (!board.is_square_attacked(Square::E1, enemy_side) && !board.is_square_attacked(Square::D1, enemy_side)) {
                            move::Move m;
                            m.set_move(Square::E1, Square::C1, piece, move::Flag::CASTLING, move::Promotion::QUEEN);
                            moves.push_back(m);
                        }
                    }
                }
                break;
            case Color::BLACK:
                king = board.king<Color::BLACK>();
                piece = Piece::bK;
                if (caslting_rights & CASTLE_BLACK_KINGSIDE) {
                    if (!bits::get_bit(occupancy, Square::F8) && !bits::get_bit(occupancy, Square::G8)) {
                        if (!board.is_square_attacked(Square::E8, enemy_side) && !board.is_square_attacked(Square::F8, enemy_side)) {
                            move::Move m;
                            m.set_move(Square::E8, Square::G8, piece, move::Flag::CASTLING, move::Promotion::QUEEN);
                            moves.push_back(m);
                        }
                    }
                } else if (caslting_rights & CASTLE_BLACK_QUEENSIDE) {
                    if (!bits::get_bit(occupancy, Square::D8) && !bits::get_bit(occupancy, Square::C8) && !bits::get_bit(occupancy, Square::B8)) {
                        if (!board.is_square_attacked(Square::E8, enemy_side) && !board.is_square_attacked(Square::D8, enemy_side)) {
                            move::Move m;
                            m.set_move(Square::E8, Square::C8, piece, move::Flag::CASTLING, move::Promotion::QUEEN);
                            moves.push_back(m);
                        }
                    }
                }
                break;
            default:
                assert(false);
                break;
        }
    }

    void generate_knight_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard knights;
        Piece piece;
        Color enemy_side;
        switch (side) {
            case Color::WHITE:
                knights = board.knights<Color::WHITE>();
                piece = Piece::wN;
                enemy_side = Color::BLACK;
                break;
            case Color::BLACK:
                knights = board.knights<Color::BLACK>();
                piece = Piece::bN;
                enemy_side = Color::WHITE;
                break;
            default:
                assert(false);
                break;
        }
        while (knights) {
            move::Move m;
            Square source = static_cast<Square>(bits::pop_bit(knights));
            Bitboard attacks = attacks::get_knight_attacks(source) & ~board.color_occupancy(side);
            while (attacks) {
                Square target = static_cast<Square>(bits::pop_bit(attacks));
                if (!bits::get_bit(board.color_occupancy(enemy_side), target)) {
                    m.set_move(source, target, piece, move::Flag::NORMAL, move::Promotion::QUEEN);
                } else {
                    m.set_move(source, target, piece, move::Flag::CAPTURE, move::Promotion::QUEEN);
                }
                moves.push_back(m);
            }
        }
    }

    void generate_bishop_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard bishops;
        Piece piece;
        Color enemy_side;
        switch (side) {
            case Color::WHITE:
                bishops = board.bishops<Color::WHITE>();
                piece = Piece::wB;
                enemy_side = Color::BLACK;
                break;
            case Color::BLACK:
                bishops = board.bishops<Color::BLACK>();
                piece = Piece::bB;
                enemy_side = Color::WHITE;
                break;
            default:
                assert(false);
                break;
        }
        while (bishops) {
            move::Move m;
            Square source = static_cast<Square>(bits::pop_bit(bishops));
            Bitboard attacks = attacks::get_bishop_attacks(source, board.occupancy()) & ~board.color_occupancy(side);
            while (attacks) {
                Square target = static_cast<Square>(bits::pop_bit(attacks));
                if (!bits::get_bit(board.color_occupancy(enemy_side), target)) {
                    m.set_move(source, target, piece, move::Flag::NORMAL, move::Promotion::QUEEN);
                } else {
                    m.set_move(source, target, piece, move::Flag::CAPTURE, move::Promotion::QUEEN);
                }
                moves.push_back(m);
            }
        }
    }
    
    void generate_rook_moves(Board board, std::vector<move::Move>& moves, Color side) {
        Bitboard rooks;
        Piece piece;
        Color enemy_side;
        switch (side) {
            case Color::WHITE:
                rooks = board.rooks<Color::WHITE>();
                piece = Piece::wR;
                enemy_side = Color::BLACK;
                break;
            case Color::BLACK:
                rooks = board.rooks<Color::BLACK>();
                piece = Piece::bR;
                enemy_side = Color::WHITE;
                break;
            default:
                assert(false);
                break;
        }
        while (rooks) {
            move::Move m;
            Square source = static_cast<Square>(bits::pop_bit(rooks));
            Bitboard attacks = attacks::get_rook_attacks(source, board.occupancy()) & ~board.color_occupancy(side);
            while (attacks) {
                Square target = static_cast<Square>(bits::pop_bit(attacks));
                if (!bits::get_bit(board.color_occupancy(enemy_side), target)) {
                    m.set_move(source, target, piece, move::Flag::NORMAL, move::Promotion::QUEEN);
                } else {
                    m.set_move(source, target, piece, move::Flag::CAPTURE, move::Promotion::QUEEN);
                }
                moves.push_back(m);
            }
        }
    }
    
    std::vector<move::Move> generate_moves(Board board) {
        std::vector<move::Move> moves;
        moves.reserve(MAX_MOVES);
        Color side = board.get_side_to_move();

        // Generate Pawn Moves
        generate_quiet_pawn_moves(board, moves, side);
        generate_capture_pawn_moves(board, moves, side);
        generate_enpassant_pawn_moves(board, moves, side);

        // Generate King Moves
        generate_castling_moves(board, moves, side);

        // Generate Knight Moves
        generate_knight_moves(board, moves, side);

        // Generate Bishop Moves
        generate_bishop_moves(board, moves, side);

        // Generate Rook Moves
        generate_rook_moves(board, moves, side);
        return moves;
    }
}