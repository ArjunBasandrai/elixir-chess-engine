#pragma once

#include "types.h"

constexpr int INPUT_WEIGHTS = 768;
constexpr int HIDDEN_SIZE   = 1024;
constexpr int SCALE         = 400;
constexpr int L1Q           = 255;
constexpr int OutputQ       = 64;

constexpr int OUTPUT_BUCKETS = 8;
constexpr int BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;