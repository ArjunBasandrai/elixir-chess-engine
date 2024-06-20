#pragma once

#include <string>

#include "types.h"

namespace elixir {
    enum class Color : I8 { WHITE, BLACK, BOTH };

    enum class Piece { wP, bP, wN, bN, wB, bB, wR, bR, wQ, bQ, wK, bK, NO_PIECE };

    enum class PieceType : I8 { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE_TYPE };

    enum Rank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, NO_RANK };

    enum File { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE };

    enum class Square : U8 {
        A1,
        B1,
        C1,
        D1,
        E1,
        F1,
        G1,
        H1,
        A2,
        B2,
        C2,
        D2,
        E2,
        F2,
        G2,
        H2,
        A3,
        B3,
        C3,
        D3,
        E3,
        F3,
        G3,
        H3,
        A4,
        B4,
        C4,
        D4,
        E4,
        F4,
        G4,
        H4,
        A5,
        B5,
        C5,
        D5,
        E5,
        F5,
        G5,
        H5,
        A6,
        B6,
        C6,
        D6,
        E6,
        F6,
        G6,
        H6,
        A7,
        B7,
        C7,
        D7,
        E7,
        F7,
        G7,
        H7,
        A8,
        B8,
        C8,
        D8,
        E8,
        F8,
        G8,
        H8,
        NO_SQ
    };

    static inline Square sq(int i) {
        return static_cast<Square>(i);
    }

    enum CastlingRights : U8 {
        CASTLE_WHITE_KINGSIDE  = 1,
        CASTLE_WHITE_QUEENSIDE = 2,
        CASTLE_BLACK_KINGSIDE  = 4,
        CASTLE_BLACK_QUEENSIDE = 8,
        ALL                    = 15
    };

    enum Phase : PhaseType { OPENING, ENDGAME, MIDDLEGAME };

    const std::string piece_str = "PpNnBbRrQqKk";

    constexpr I8 color_offset[2]    = {1, -1};
    constexpr int PromotionRank[2]  = {RANK_8, RANK_1};
    constexpr int DoublePawnRank[2] = {RANK_2, RANK_7};

    constexpr Bitboard Rank_1_BB = 0x00000000000000FF;
    constexpr Bitboard Rank_2_BB = 0x000000000000FF00;
    constexpr Bitboard Rank_3_BB = 0x0000000000FF0000;
    constexpr Bitboard Rank_4_BB = 0x00000000FF000000;
    constexpr Bitboard Rank_5_BB = 0x000000FF00000000;
    constexpr Bitboard Rank_6_BB = 0x0000FF0000000000;
    constexpr Bitboard Rank_7_BB = 0x00FF000000000000;
    constexpr Bitboard Rank_8_BB = 0xFF00000000000000;

    constexpr Bitboard not_a_file  = 18374403900871474942ULL;
    constexpr Bitboard not_h_file  = 9187201950435737471ULL;
    constexpr Bitboard not_gh_file = 4557430888798830399ULL;
    constexpr Bitboard not_ab_file = 18229723555195321596ULL;

    constexpr Bitboard FILE_A_BB = 0x0101010101010101ULL;
    constexpr Bitboard FILE_B_BB = 0x0202020202020202ULL;
    constexpr Bitboard FILE_C_BB = 0x0404040404040404ULL;
    constexpr Bitboard FILE_D_BB = 0x0808080808080808ULL;
    constexpr Bitboard FILE_E_BB = 0x1010101010101010ULL;
    constexpr Bitboard FILE_F_BB = 0x2020202020202020ULL;
    constexpr Bitboard FILE_G_BB = 0x4040404040404040ULL;
    constexpr Bitboard FILE_H_BB = 0x8080808080808080ULL;

    constexpr Bitboard Files[8] = {FILE_A_BB, FILE_B_BB, FILE_C_BB, FILE_D_BB,
                                   FILE_E_BB, FILE_F_BB, FILE_G_BB, FILE_H_BB};

    constexpr Bitboard Ranks[8] = {Rank_1_BB, Rank_2_BB, Rank_3_BB, Rank_4_BB,
                                   Rank_5_BB, Rank_6_BB, Rank_7_BB, Rank_8_BB};

    constexpr I8 MAX_DEPTH = 64;
    constexpr I8 MAX_PLY   = 64;

    // TT size terms
    constexpr int MIN_HASH          = 4;
    constexpr int DEFAULT_HASH_SIZE = 64;
    constexpr int MAX_HASH          = 1024;

    static inline int get_rank(Square sq) {
        return (static_cast<int>(sq) >> 3) & 7;
    }

    static inline int get_file(Square sq) {
        return static_cast<int>(sq) & 7;
    }

    static inline int get_square_distance(Square a, Square b) {
        return std::max(std::abs(get_rank(a) - get_rank(b)), std::abs(get_file(a) - get_file(b)));
    }

    constexpr I32 INF        = 32001;
    constexpr I32 MATE       = 32000;
    constexpr I32 MATE_FOUND = MATE - MAX_PLY;
}