#define _CRT_SECURE_NO_WARNINGS
#include <cassert>
#include <cstring>

#include "defs.h"
#include "types.h"
#include "utils/bits.h"

#include "nnue.h"
#include "simd.h"
#include "third_party/incbin.h"

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

    inline auto get_index(const Piece piece, const Square sq) {
        const int color     = static_cast<int>(piece_color(piece));
        const int piecetype = static_cast<int>(piece_to_piecetype(piece));
        const int white_sq  = static_cast<int>(sq);
        const int black_sq  = white_sq ^ 56;

        const int white_input_index = color * 384 + piecetype * 64 + white_sq;
        const int black_input_index = (color ^ 1) * 384 + piecetype * 64 + black_sq;

        return std::make_pair(white_input_index, black_input_index);
    }

    void Accumulator::add(const Piece piece, const Square sq, Network &net) {
        const auto [white_input_index, black_input_index] = get_index(piece, sq);

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[0][i] += net.layer_1_weights[white_input_index][i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[1][i] += net.layer_1_weights[black_input_index][i];
        }
    }

    void Accumulator::remove(const Piece piece, const Square sq, Network &net) {
        const auto [white_input_index, black_input_index] = get_index(piece, sq);

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[0][i] -= net.layer_1_weights[white_input_index][i];
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulator[1][i] -= net.layer_1_weights[black_input_index][i];
        }
    }

    void NNUE::add_sub(const Piece add_piece, const Square add_square, const Piece sub_piece, const Square sub_square) {
        const auto [white_add_index, black_add_index] = get_index(add_piece, add_square);
        const auto [white_sub_index, black_sub_index] = get_index(sub_piece, sub_square);

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][0][i] = accumulators[current_acc - 1][0][i] + (net.layer_1_weights[white_add_index][i] - net.layer_1_weights[white_sub_index][i]);
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][1][i] = accumulators[current_acc - 1][1][i] + (net.layer_1_weights[black_add_index][i] - net.layer_1_weights[black_sub_index][i]);
        }
    }

    void NNUE::add_sub_sub(const Piece add_piece, const Square add_square, const Piece sub_piece1, const Square sub_square1, const Piece sub_piece2, const Square sub_square2) {
        const auto [white_add_index, black_add_index] = get_index(add_piece, add_square);
        const auto [white_sub_index1, black_sub_index1] = get_index(sub_piece1, sub_square1);
        const auto [white_sub_index2, black_sub_index2] = get_index(sub_piece2, sub_square2);

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][0][i] = accumulators[current_acc - 1][0][i] + (net.layer_1_weights[white_add_index][i] - net.layer_1_weights[white_sub_index1][i] - net.layer_1_weights[white_sub_index2][i]);
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][1][i] = accumulators[current_acc - 1][1][i] + (net.layer_1_weights[black_add_index][i] - net.layer_1_weights[black_sub_index1][i] - net.layer_1_weights[black_sub_index2][i]);
        }
    }

    void NNUE::add_add_sub_sub(const Piece add_piece1, const Square add_square1, const Piece add_piece2, const Square add_square2, const Piece sub_piece1, const Square sub_square1, const Piece sub_piece2, const Square sub_square2) {
        const auto [white_add_index1, black_add_index1] = get_index(add_piece1, add_square1);
        const auto [white_add_index2, black_add_index2] = get_index(add_piece2, add_square2);
        const auto [white_sub_index1, black_sub_index1] = get_index(sub_piece1, sub_square1);
        const auto [white_sub_index2, black_sub_index2] = get_index(sub_piece2, sub_square2);

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][0][i] = accumulators[current_acc - 1][0][i] + (net.layer_1_weights[white_add_index1][i] + net.layer_1_weights[white_add_index2][i] - net.layer_1_weights[white_sub_index1][i] - net.layer_1_weights[white_sub_index2][i]);
        }

        for (int i = 0; i < HIDDEN_SIZE; i++) {
            accumulators[current_acc][1][i] = accumulators[current_acc - 1][1][i] + (net.layer_1_weights[black_add_index1][i] + net.layer_1_weights[black_add_index2][i] - net.layer_1_weights[black_sub_index1][i] - net.layer_1_weights[black_sub_index2][i]);
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
            read += fread(untransposed_output_weights, sizeof(I16),
                          HIDDEN_SIZE * OUTPUT_BUCKETS * 2, nn);
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

            std::memcpy(untransposed_output_weights, &gEVALData[memoryIndex],
                        HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(I16) * 2);

            memoryIndex += HIDDEN_SIZE * OUTPUT_BUCKETS * sizeof(I16) * 2;
            std::memcpy(net.output_bias, &gEVALData[memoryIndex], OUTPUT_BUCKETS * sizeof(I16));
        }

        for (int stm = 0; stm < 2; stm++) {
            for (int weight = 0; weight < HIDDEN_SIZE; weight++) {
                for (int bucket = 0; bucket < OUTPUT_BUCKETS; bucket++) {
                    net.output_weights[bucket][stm][weight] =
                        untransposed_output_weights[stm][weight][bucket];
                }
            }
        }
    }

    int NNUE::eval(const Color side, const int bucket) {
        I8 icolor = static_cast<I8>(side);
        int eval  = 0;

#if defined(USE_SIMD)
        vepi32 sum               = zero_epi32();
        constexpr int chunk_size = sizeof(vepi16) / sizeof(int16_t);

        for (int i = 0; i < HIDDEN_SIZE; i += chunk_size) {
            const vepi16 accumulator_data = load_epi16(&accumulators[current_acc][icolor][i]);
            const vepi16 weights          = load_epi16(&net.output_weights[bucket][0][i]);

            const vepi16 clipped_accumulator = clip(accumulator_data, L1Q);

            const vepi16 intermediate = multiply_epi16(clipped_accumulator, weights);

            const vepi32 result = multiply_add_epi16(intermediate, clipped_accumulator);

            sum = add_epi32(sum, result);
        }

        for (int i = 0; i < HIDDEN_SIZE; i += chunk_size) {
            const vepi16 accumulator_data = load_epi16(&accumulators[current_acc][icolor ^ 1][i]);
            const vepi16 weights          = load_epi16(&net.output_weights[bucket][1][i]);

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
            eval +=
                screlu(accumulators[current_acc][icolor ^ 1][i]) * net.output_weights[bucket][1][i];
        }

#endif
        eval /= L1Q;
        eval += net.output_bias[bucket];
        eval = (eval * SCALE) / (L1Q * OutputQ);

        return eval;
    }
}