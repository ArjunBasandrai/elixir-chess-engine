#pragma once

#include "arch.h"
#include "types.h"

namespace elixir::nnue {
    struct Network {
        alignas(64) I16 layer_1_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
        alignas(64) I16 layer_1_biases[HIDDEN_SIZE];
        alignas(64) I16 output_weights[OUTPUT_BUCKETS][2][HIDDEN_SIZE];
        alignas(64) I16 output_bias[OUTPUT_BUCKETS];
    };
}