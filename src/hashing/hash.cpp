#include <cstdlib>
#include <ctime>

#include "../board/board.h"
#include "../defs.h"
#include "../types.h"
#include "../utils/bits.h"
#include "hash.h"

namespace elixir {
    namespace random {
        U64 seed = 12436596276006385841ULL;
        U64 random_u64() {
            U64 num = seed;
            num ^= num << 13;
            num ^= num >> 17;
            num ^= num << 5;
            seed = num;
            return num;
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