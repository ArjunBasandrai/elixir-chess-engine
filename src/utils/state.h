#pragma once

#include "defs.h"
#include "move.h"
#include "types.h"

namespace elixir {
    struct State {
        State() = default;
        State(const U64 &hash_key, const U64 &pawn_hash, const Castling &castling_rights, const Square &enpass,
              const I8 &fifty_move_counter, const Piece &captured_piece, const EvalScore &eval)
            : hash_key(hash_key), pawn_hash(pawn_hash), castling_rights(castling_rights), enpass(enpass),
              fifty_move_counter(fifty_move_counter), captured_piece(captured_piece), eval(eval) {}
        U64 hash_key;
        U64 pawn_hash;
        Castling castling_rights;
        Square enpass;
        I8 fifty_move_counter;
        Piece captured_piece;
        EvalScore eval;
    };
}