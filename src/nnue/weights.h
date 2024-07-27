#pragma once

#include "arch.h"
#include "../types.h"

namespace elixir::nnue {
    struct Network {
        I16 layer_1_weights[INPUT_WEIGHTS][HIDDEN_SIZE];
        I16 layer_1_biases[HIDDEN_SIZE];
        I16 output_weights[2][HIDDEN_SIZE];
        I16 output_bias;
    };
}