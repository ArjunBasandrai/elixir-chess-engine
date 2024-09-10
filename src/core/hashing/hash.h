#pragma once

#include "../types.h"

namespace elixir {
    namespace random {
        extern U64 seed;
        U64 random_u64();
    }

    namespace zobrist {
        extern U64 piece_keys[12][64];
        extern U64 side_key;
        extern U64 castle_keys[16];
        extern U64 ep_keys[64];

        void init_hash_keys();
    }
}