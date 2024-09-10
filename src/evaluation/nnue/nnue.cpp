#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <cassert>

#include "defs.h"
#include "types.h"
#include "utils/bits.h"

#include "third_party/incbin.h"
#include "nnue.h"
#include "simd.h"

#include "board/board.h"

#if ! defined(_MSC_VER)
INCBIN(EVAL, EVALFILE);
#else
const unsigned char gEVALData[1]    = {};
const unsigned char *const gEVALEnd = &gEVALData[1];
const unsigned int gEVALSize        = 1;
#endif

namespace elixir::nnue {
    inline int screlu(I16 value) {
        const int clipped = std::clamp<int>(static_cast<int>(value), 0, L1Q);
        return clipped * clipped;
    }

    void Accumulator::set_position(const Board &board, Network& net) {
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[0][i] = net.layer_1_biases[i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[1][i] = net.layer_1_biases[i];
        }

        for (int color = 0; color < 2; color++) {
            for (int piece = 0; piece < 6; piece++) {
                Bitboard bb = board.color_occupancy(color) &
                              board.piece_bitboard(static_cast<PieceType>(piece));
                while (bb) {
                    const int white_sq = bits::pop_bit(bb);
                    const int black_sq = white_sq ^ 56;

                    const int white_input_index = color * 384 + piece * 64 + white_sq;
                    const int black_input_index = (color ^ 1) * 384 + piece * 64 + black_sq;

                    for (int i = 0; i < HIDDEN_SIZE; i++) {
                        accumulator[0][i] += net.layer_1_weights[white_input_index][i];
                    }

                    for (int i = 0; i < HIDDEN_SIZE; i++) {
                        accumulator[1][i] += net.layer_1_weights[black_input_index][i];
                    }
                }
            }
        }
    }

    void Accumulator::add(const Piece piece, const Square sq, Network& net) {
        const int color = static_cast<int>(piece_color(piece));
        const int piecetype = static_cast<int>(piece_to_piecetype(piece));
        const int white_sq = static_cast<int>(sq);
        const int black_sq = white_sq ^ 56;

        const int white_input_index = color * 384 + piecetype * 64 + white_sq;
        const int black_input_index = (color ^ 1) * 384 + piecetype * 64 + black_sq;

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[0][i] += net.layer_1_weights[white_input_index][i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[1][i] += net.layer_1_weights[black_input_index][i];
        }
    }

    void Accumulator::remove(const Piece piece, const Square sq, Network& net) {
        const int color = static_cast<int>(piece_color(piece));
        const int piecetype = static_cast<int>(piece_to_piecetype(piece));
        const int white_sq = static_cast<int>(sq);
        const int black_sq = white_sq ^ 56;

        const int white_input_index = color * 384 + piecetype * 64 + white_sq;
        const int black_input_index = (color ^ 1) * 384 + piecetype * 64 + black_sq;

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[0][i] -= net.layer_1_weights[white_input_index][i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[1][i] -= net.layer_1_weights[black_input_index][i];
        }
    }

    void Accumulator::make_move(const Board& board, const move::Move& move, Network& net) {
        const Square from = move.get_from();
        const Square to = move.get_to();
        const move::Flag flag = move.get_flag();
        const Piece piece = move.get_piece();
        const Color color = board.get_side_to_move();
        const int stm = static_cast<int>(color);

        if (move.is_promotion()) {
            const Piece promo = [&] {
                const int promo_piece = move.get_promo_piece();
                switch (promo_piece) {
                    case 1:
                        return (color == Color::WHITE) ? Piece::wN : Piece::bN;
                    case 2:
                        return (color == Color::WHITE) ? Piece::wB : Piece::bB;
                    case 3:
                        return (color == Color::WHITE) ? Piece::wR : Piece::bR;
                    case 4:
                        return (color == Color::WHITE) ? Piece::wQ : Piece::bQ;
                    default:
                        assert(0);
                        exit(1);
                }
            } ();

            remove(piece, from, net);

            if (move.is_capture()) {
                const auto captured = board.piece_on(to);
                remove(captured, to, net);
            }

            add(promo, to, net);

            return;
        }

        if (move.is_en_passant()) {
            const int enpass_sq = static_cast<int>(board.get_en_passant_square());
            Square captured_square = static_cast<Square>(enpass_sq - 8 * color_offset[stm]);
            remove(piece_color(piece) == Color::WHITE ? Piece::bP : Piece::wP, captured_square, net);
        }

        if (move.is_capture()) {
            const auto captured = board.piece_on(to);
            remove(captured, to, net);
        }

        remove(piece, from, net);
        add(piece, to, net);

        if (move.is_castling()) {
            const Square rook_from = [&] {
                if (to == Square::G1 || to == Square::G8) {
                    return static_cast<Square>(static_cast<int>(to) + 1);
                } else {
                    return static_cast<Square>(static_cast<int>(to) - 2);
                }
            } ();

            const Square rook_to = [&] {
                if (to == Square::G1 || to == Square::G8) {
                    return static_cast<Square>(static_cast<int>(to) - 1);
                } else {
                    return static_cast<Square>(static_cast<int>(to) + 1);
                }
            } ();

            remove(piece_color(piece) == Color::WHITE ? Piece::wR : Piece::bR, rook_from, net);
            add(piece_color(piece) == Color::WHITE ? Piece::wR : Piece::bR, rook_to, net);
        }
    }

    void NNUE::init(const std::string file) {
        FILE *nn = fopen(file.c_str(), "rb");

        I16 untransposed_output_weights[2][HIDDEN_SIZE][OUTPUT_BUCKETS];

        if (nn) {
            // initialize an accumulator for every input of the second layer
            size_t read            = 0;
            size_t fileSize        = sizeof(Network);
            size_t objectsExpected = fileSize / sizeof(I16);

            read += fread(net.layer_1_weights, sizeof(I16), INPUT_WEIGHTS * HIDDEN_SIZE, nn);
            read += fread(net.layer_1_biases, sizeof(I16), HIDDEN_SIZE, nn);
            read += fread(net.output_weights, sizeof(I16), HIDDEN_SIZE * 2, nn);
            read += fread(untransposed_output_weights, sizeof(I16), HIDDEN_SIZE * OUTPUT_BUCKETS * 2, nn);
            read += fread(net.output_bias, sizeof(I16), OUTPUT_BUCKETS, nn);

            if (read != objectsExpected) {
                std::cout << "Error loading the net, aborting ";
                std::cout << "Expected " << objectsExpected << " shorts, got " << read << "\n";
                exit(1);
            }

            // after reading the config we can close the file
            fclose(nn);
        } else {
            // if we don't find the nnue file we use the net embedded in the exe
            uint64_t memoryIndex = 0;

            std::memcpy(net.layer_1_weights, &gEVALData[memoryIndex],
                        INPUT_WEIGHTS * HIDDEN_SIZE * sizeof(I16));
            memoryIndex += INPUT_WEIGHTS * HIDDEN_SIZE * sizeof(I16);

            std::memcpy(net.layer_1_biases, &gEVALData[memoryIndex], HIDDEN_SIZE * sizeof(I16));
            memoryIndex += HIDDEN_SIZE * sizeof(I16);

            std::memcpy(untransposed_output_weights, &gEVALData[memoryIndex], HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(I16) * 2);

            memoryIndex += HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(I16) * 2;
            std::memcpy(net.output_bias, &gEVALData[memoryIndex], OUTPUT_BUCKETS * sizeof(I16));
        }

        for (int stm = 0; stm < 2; stm++) {
            for (int weight = 0; weight < HIDDEN_SIZE; weight++) {
                for (int bucket = 0; bucket < OUTPUT_BUCKETS; bucket++) {
                    net.output_weights[bucket][stm][weight] = untransposed_output_weights[stm][weight][bucket];
                }
            }
        }
    }

    void NNUE::set_position(const Board &board) {
        accumulators[current_acc].set_position(board, net);
    }

    int NNUE::eval(const Color side, const int bucket) {
        I8 icolor = static_cast<I8>(side);
        int eval  = 0;
        
#if defined(USE_SIMD)
        vepi32 sum = zero_epi32();
        constexpr int chunk_size = sizeof(vepi16) / sizeof(int16_t);

        for (int i = 0; i < HIDDEN_SIZE; i += chunk_size) {
            const vepi16 accumulator_data = load_epi16(&accumulators[current_acc][icolor][i]);
            const vepi16 weights = load_epi16(&net.output_weights[bucket][0][i]);

            const vepi16 clipped_accumulator = clip(accumulator_data, L1Q);

            const vepi16 intermediate = multiply_epi16(clipped_accumulator, weights);

            const vepi32 result = multiply_add_epi16(intermediate, clipped_accumulator);

            sum = add_epi32(sum, result);
        }

        for (int i = 0; i < HIDDEN_SIZE; i += chunk_size) {
            const vepi16 accumulator_data = load_epi16(&accumulators[current_acc][icolor ^ 1][i]);
            const vepi16 weights = load_epi16(&net.output_weights[bucket][1][i]);

            const vepi16 clipped_accumulator = clip(accumulator_data, L1Q);

            const vepi16 intermediate = multiply_epi16(clipped_accumulator, weights);

            const vepi32 result = multiply_add_epi16(intermediate, clipped_accumulator);

            sum = add_epi32(sum, result);
        }

        eval = reduce_add_epi32(sum);
#else
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            eval += screlu(accumulators[current_acc][icolor][i]) * net.output_weights[bucket][0][i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            eval += screlu(accumulators[current_acc][icolor ^ 1][i]) * net.output_weights[bucket][1][i];
        }

#endif
        eval /= L1Q;
        eval += net.output_bias[bucket];
        eval = (eval * SCALE) / (L1Q * OutputQ);

        return eval;
    }
}