#pragma once

#include "../types.h"


namespace elixir {
    constexpr U16 INPUT_WEIGHTS = 768;
    constexpr U16 HIDDEN_SIZE   = 1024;
    constexpr U16 SCALE         = 400;
    constexpr U16 L1Q           = 255;
    constexpr U8 OutputQ       = 64;

    constexpr U8 OUTPUT_BUCKETS = 8;
    constexpr U8 BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;

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