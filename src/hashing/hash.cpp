#include <cstdlib>
#include <ctime>

#include "hash.h"
#include "../types.h"
#include "../defs.h"
#include "../board/board.h"
#include "../utils/bits.h"

namespace elixir {
    namespace random {
        U64 random_u64() {
            return (U64)rand() + ((U64)rand() << 15) + ((U64)rand() << 30) + ((U64)rand() << 45) + (((U64)rand() & 0xf) << 60);
        }
    }
    namespace zobrist {
        U64 piece_keys[12][64];
        U64 side_key;
        U64 castle_keys[16];
        U64 ep_keys[64];

        void init_hash_keys() {
            srand(123);
            for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 64; j++) {
                    piece_keys[i][j] = random::random_u64();
                }
            }
            side_key = random::random_u64();
            for (int i = 0; i < 16; i++) {
                castle_keys[i] = random::random_u64();
            }
            for (int i = 0; i < 64; i++) {
                ep_keys[i] = random::random_u64();
            }
        }
    }

    U64 Board::get_board_hash() {
        U64 hash = 0ULL;
        for (int piece = 0; piece < 6; piece++) {
            Bitboard bitboard = piece_bitboard(static_cast<PieceType>(piece));
            for (int color = 0; color < 2; color++) {
                Bitboard color_bitboard = bitboard & b_occupancies[color];
                while (color_bitboard) {
                    int square = bits::pop_bit(color_bitboard);
                    hash ^= zobrist::piece_keys[piece + color * 6][square];
                }
            }
        }
        if (side == Color::BLACK) {
            hash ^= zobrist::side_key;
        }
        hash ^= zobrist::castle_keys[castling_rights];
        if (en_passant_square != Square::NO_SQ) {
            hash ^= zobrist::ep_keys[static_cast<I8>(en_passant_square)];
        }
        return hash;
    }
}