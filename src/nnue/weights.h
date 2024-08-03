#pragma once

#include "arch.h"
#include "../types.h"

namespace elixir::nnue {
    struct Network {
        alignas(32) I16 layer_1_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
        alignas(32) I16 layer_1_biases[HIDDEN_SIZE];
        alignas(32) I16 output_weights[2][HIDDEN_SIZE];
        alignas(32) I16 output_bias;
    };
}