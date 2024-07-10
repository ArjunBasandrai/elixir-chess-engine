#pragma once

#include <array>
#include <cstdint>

namespace elixir {
    using Bitboard = std::uint64_t;

    using U8  = std::uint8_t;
    using U16 = std::uint16_t;
    using U32 = std::uint32_t;
    using U64 = std::uint64_t;

    using I8  = std::int8_t;
    using I16 = std::int16_t;
    using I32 = std::int32_t;
    using I64 = std::int64_t;

    using F32 = float;
    using F64 = double;

    using Castling  = std::uint8_t;
    using Move_T    = std::uint32_t;
    using EvalScore = std::int32_t;
    using Score     = std::int16_t;
    using PhaseType = std::uint8_t;

    using ContHistEntry = std::array<std::array<int, 64>, 12>;
    using ContHistArray = std::array<std::array<ContHistEntry, 64>, 12>;
}