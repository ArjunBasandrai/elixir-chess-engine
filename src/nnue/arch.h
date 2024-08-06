#pragma once

#include "../types.h"

namespace elixir {
    constexpr int INPUT_WEIGHTS = 768;
    constexpr int HIDDEN_SIZE   = 1024;
    constexpr int SCALE         = 400;
    constexpr int L1Q           = 255;
    constexpr int OutputQ       = 64;

    constexpr int OUTPUT_BUCKETS = 8;
    constexpr int BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;

    constexpr U8 KING_BUCKETS = 7;
    constexpr std::array<U8, 64> KING_BUCKETS_SCHEME = {
        0, 0, 1, 1, 1, 1, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6
    };
}