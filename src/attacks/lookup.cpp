#include <array>
#include <cassert>

#include "lookup.h"

#include "../defs.h"
#include "../hashing/hash.h"
#include "../utils/bits.h"
#include "attacks.h"
#include "magics.h"

namespace elixir::attacks {
    Bitboard pawn_attacks[2][64];
    Bitboard knight_attacks[64];
    Bitboard king_attacks[64];

    Bitboard mask_pawn_attacks(Square sq, Color side) {
        Bitboard bb   = 0ULL;
        Bitboard mask = 0ULL;
        bits::set_bit(bb, sq);
        int rank = get_rank(sq);
        int file = get_file(sq);
        switch (side) {
            case Color::WHITE:
                mask |= (bb << 7) & not_h_file;
                mask |= (bb << 9) & not_a_file;
                break;
            case Color::BLACK:
                mask |= (bb >> 7) & not_a_file;
                mask |= (bb >> 9) & not_h_file;
                break;
            default:
                break;
        }
        return mask;
    }

    Bitboard mask_knight_attacks(Square sq) {
        Bitboard bb   = 0ULL;
        Bitboard mask = 0ULL;
        bits::set_bit(bb, sq);
        int rank = get_rank(sq);
        int file = get_file(sq);
        mask |= (bb << 17) & not_a_file;
        mask |= (bb << 15) & not_h_file;
        mask |= (bb << 10) & (not_ab_file);
        mask |= (bb << 6) & (not_gh_file);
        mask |= (bb >> 17) & not_h_file;
        mask |= (bb >> 15) & not_a_file;
        mask |= (bb >> 10) & (not_gh_file);
        mask |= (bb >> 6) & (not_ab_file);
        return mask;
    }

    Bitboard mask_king_attacks(Square sq) {
        Bitboard bb   = 0ULL;
        Bitboard mask = 0ULL;
        bits::set_bit(bb, sq);
        int rank = get_rank(sq);
        int file = get_file(sq);
        mask |= (bb << 8);
        mask |= (bb << 9) & not_a_file;
        mask |= (bb << 7) & not_h_file;
        mask |= (bb << 1) & not_a_file;

        mask |= (bb >> 8);
        mask |= (bb >> 9) & not_h_file;
        mask |= (bb >> 7) & not_a_file;
        mask |= (bb >> 1) & not_h_file;

        return mask;
    }

    void init_pawn_attacks() {
        for (int i = 0; i < 64; i++) {
            pawn_attacks[static_cast<int>(Color::WHITE)][i] =
                mask_pawn_attacks(static_cast<Square>(i), Color::WHITE);
            pawn_attacks[static_cast<int>(Color::BLACK)][i] =
                mask_pawn_attacks(static_cast<Square>(i), Color::BLACK);
        }
    }

    void init_knight_attacks() {
        for (int i = 0; i < 64; i++) {
            knight_attacks[i] = mask_knight_attacks(static_cast<Square>(i));
        }
    }

    void init_king_attacks() {
        for (int i = 0; i < 64; i++) {
            king_attacks[i] = mask_king_attacks(static_cast<Square>(i));
        }
    }

}

namespace elixir::magic {

    /*
    U64 bishop_magic_numbers[64];
    U64 rook_magic_numbers[64];
    */

    U64 bishop_masks[64];
    U64 rook_masks[64];
    U64 bishop_attacks[64][512];
    U64 rook_attacks[64][4096];

    Bitboard mask_bishop_attacks(Square sq) {
        Bitboard mask = 0ULL;
        int rank      = get_rank(sq);
        int file      = get_file(sq);
        int r, f;
        for (r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
        }
        for (r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
        }
        for (r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
        }
        for (r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
        }
        return mask;
    }

    Bitboard mask_rook_attacks(Square sq) {
        Bitboard mask = 0ULL;
        int rank      = get_rank(sq);
        int file      = get_file(sq);
        int r, f;
        for (r = rank + 1; r <= 6; r++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + file));
        }
        for (r = rank - 1; r >= 1; r--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + file));
        }
        for (f = file + 1; f <= 6; f++) {
            bits::set_bit(mask, static_cast<Square>(rank * 8 + f));
        }
        for (f = file - 1; f >= 1; f--) {
            bits::set_bit(mask, static_cast<Square>(rank * 8 + f));
        }
        return mask;
    }

    Bitboard bishop_attacks_on_the_fly(Square sq, Bitboard block) {
        Bitboard mask = 0ULL;
        int rank      = get_rank(sq);
        int file      = get_file(sq);
        int r, f;
        for (r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + f))) {
                break;
            }
        }
        for (r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + f))) {
                break;
            }
        }
        for (r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + f))) {
                break;
            }
        }
        for (r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + f))) {
                break;
            }
        }
        return mask;
    }

    Bitboard rook_attacks_on_the_fly(Square sq, Bitboard block) {
        Bitboard mask = 0ULL;
        int rank      = get_rank(sq);
        int file      = get_file(sq);
        int r, f;
        for (r = rank + 1; r <= 7; r++) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + file));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + file))) {
                break;
            }
        }
        for (r = rank - 1; r >= 0; r--) {
            bits::set_bit(mask, static_cast<Square>(r * 8 + file));
            if (bits::get_bit(block, static_cast<Square>(r * 8 + file))) {
                break;
            }
        }
        for (f = file + 1; f <= 7; f++) {
            bits::set_bit(mask, static_cast<Square>(rank * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(rank * 8 + f))) {
                break;
            }
        }
        for (f = file - 1; f >= 0; f--) {
            bits::set_bit(mask, static_cast<Square>(rank * 8 + f));
            if (bits::get_bit(block, static_cast<Square>(rank * 8 + f))) {
                break;
            }
        }
        return mask;
    }

    Bitboard set_occupancy(int index, int bits, Bitboard attack_mask) {
        Bitboard occupancy = 0ULL;
        for (int i = 0; i < bits; i++) {
            int square = bits::pop_bit(attack_mask);
            if (index & (1 << i)) {
                bits::set_bit(occupancy, static_cast<Square>(square));
            }
        }
        return occupancy;
    }

    U64 get_magic_candidate() {
        U64 n1 = elixir::random::random_u64();
        U64 n2 = elixir::random::random_u64();
        U64 n3 = elixir::random::random_u64();
        return (n1 & n2 & n3);
    }

    U64 find_magic_number(Square square, int bits, PieceType piece) {
        std::array<Bitboard, 4096> occupancy_variations;
        std::array<Bitboard, 4096> attack_variations;
        std::array<Bitboard, 4096> used_attack_variations;
        Bitboard attack_mask =
            (piece == PieceType::BISHOP) ? mask_bishop_attacks(square) : mask_rook_attacks(square);
        int occupancy_indices = 1 << bits;
        for (int i = 0; i < occupancy_indices; i++) {
            occupancy_variations[i] = set_occupancy(i, bits, attack_mask);
            attack_variations[i] =
                (piece == PieceType::BISHOP)
                    ? magic::bishop_attacks_on_the_fly(square, occupancy_variations[i])
                    : magic::rook_attacks_on_the_fly(square, occupancy_variations[i]);
        }

        for (int random_count = 0; random_count < 100000000; random_count++) {
            U64 magic_number = get_magic_candidate();
            if (bits::count_bits((magic_number * attack_mask) & 0xFF00000000000000) < 6) {
                continue;
            }

            used_attack_variations.fill(0ULL);
            bool valid = true;
            int index;

            for (index = 0; index < occupancy_indices; index++) {
                int magic_index = (occupancy_variations[index] * magic_number) >> (64 - bits);
                if (used_attack_variations[magic_index] == 0ULL) {
                    used_attack_variations[magic_index] = attack_variations[index];
                } else if (used_attack_variations[magic_index] != attack_variations[index]) {
                    valid = false;
                    break;
                }
                valid = true;
            }

            if (valid) {
                return magic_number;
            }
        }
        assert(false);
        return 0ULL;
    }

    void init_bishop_attacks() {
        for (int square = 0; square < 64; square++) {
            bishop_masks[square]  = mask_bishop_attacks(static_cast<Square>(square));
            U64 attack_mask       = bishop_masks[square];
            int bits              = bishop_relevant_bits[square];
            int occupancy_indices = 1 << bits;
            for (int index = 0; index < occupancy_indices; index++) {
                U64 occupancy   = set_occupancy(index, bits, attack_mask);
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bits);
                bishop_attacks[square][magic_index] =
                    bishop_attacks_on_the_fly(static_cast<Square>(square), occupancy);
            }
        }
    }

    void init_rook_attacks() {
        for (int square = 0; square < 64; square++) {
            rook_masks[square]    = mask_rook_attacks(static_cast<Square>(square));
            U64 attack_mask       = rook_masks[square];
            int bits              = rook_relevant_bits[square];
            int occupancy_indices = 1 << bits;
            for (int index = 0; index < occupancy_indices; index++) {
                U64 occupancy   = set_occupancy(index, bits, attack_mask);
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - bits);
                rook_attacks[square][magic_index] =
                    rook_attacks_on_the_fly(static_cast<Square>(square), occupancy);
            }
        }
    }

    /*
    void init_magic_numbers() {
        for (int i = 0; i < 64; i++) {
            bishop_magic_numbers[i] = magic::find_magic_number(static_cast<Square>(i),
    bishop_relevant_bits[i], PieceType::BISHOP); rook_magic_numbers[i] =
    magic::find_magic_number(static_cast<Square>(i), rook_relevant_bits[i], PieceType::ROOK);
        }
    }
    */
}