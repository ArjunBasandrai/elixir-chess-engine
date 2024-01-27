#include <iostream>

#include "src/types.h"
#include "src/defs.h"
#include "src/bits.h"
#include "src/board.h"

using namespace elixir;

int main() {
    Board board;
    board.set_piece(Square::A1, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::A2, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::A3, PieceType::BISHOP, Color::WHITE);
    board.set_piece(Square::A4, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::A5, PieceType::KING, Color::WHITE);
    board.set_piece(Square::A6, PieceType::KING, Color::BLACK);

    print_bitboard(board.pawns());
    print_bitboard(board.occupancy());
    print_bitboard(board.white_occupancy());
    print_bitboard(board.black_occupancy());
    print_bitboard(board.all_pieces());
    print_bitboard(board.all_pieces(Color::WHITE));
    print_bitboard(board.all_pieces(Color::BLACK));
    return 0;
}