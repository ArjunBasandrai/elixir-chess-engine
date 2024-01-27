#include <iostream>

#include "src/types.h"
#include "src/defs.h"
#include "src/bits.h"
#include "src/board.h"

using namespace elixir;

int main() {
    Board board;
    board.set_piece(Square::A1, PieceType::ROOK, Color::WHITE);
    board.set_piece(Square::B1, PieceType::KNIGHT, Color::WHITE);
    board.set_piece(Square::C1, PieceType::BISHOP, Color::WHITE);
    board.set_piece(Square::D1, PieceType::QUEEN, Color::WHITE);
    board.set_piece(Square::E1, PieceType::KING, Color::WHITE);
    board.set_piece(Square::F1, PieceType::BISHOP, Color::WHITE);
    board.set_piece(Square::G1, PieceType::KNIGHT, Color::WHITE);
    board.set_piece(Square::H1, PieceType::ROOK, Color::WHITE);
    
    board.set_piece(Square::A2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::B2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::C2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::D2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::E2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::F2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::G2, PieceType::PAWN, Color::WHITE);
    board.set_piece(Square::H2, PieceType::PAWN, Color::WHITE);

    board.set_piece(Square::A7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::B7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::C7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::D7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::E7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::F7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::G7, PieceType::PAWN, Color::BLACK);
    board.set_piece(Square::H7, PieceType::PAWN, Color::BLACK);

    board.set_piece(Square::A8, PieceType::ROOK, Color::BLACK);
    board.set_piece(Square::B8, PieceType::KNIGHT, Color::BLACK);
    board.set_piece(Square::C8, PieceType::BISHOP, Color::BLACK);
    board.set_piece(Square::D8, PieceType::QUEEN, Color::BLACK);
    board.set_piece(Square::E8, PieceType::KING, Color::BLACK);
    board.set_piece(Square::F8, PieceType::BISHOP, Color::BLACK);
    board.set_piece(Square::G8, PieceType::KNIGHT, Color::BLACK);
    board.set_piece(Square::H8, PieceType::ROOK, Color::BLACK);

    board.print_board();
    return 0;
}