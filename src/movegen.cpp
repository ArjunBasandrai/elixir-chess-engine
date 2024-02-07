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

    std::vector<move::Move> generate_moves(Board board) {
        std::vector<move::Move> moves;
        moves.reserve(MAX_MOVES);
        generate_quiet_pawn_moves(board, moves, board.get_side_to_move());
        generate_capture_pawn_moves(board, moves, board.get_side_to_move());
        generate_enpassant_pawn_moves(board, moves, board.get_side_to_move());
        return moves;
    }
}