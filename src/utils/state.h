#pragma once

#include "../types.h"
#include "../defs.h"

namespace elixir {
    struct State {
        State() = default;
        State(const U64 &hash_key, const Castling &castling_rights, 
            const Square &enpass, const I8 &fifty_move_counter, const Piece &captured_piece)
            : hash_key(hash_key), 
                castling_rights(castling_rights), 
                enpass(enpass), 
                fifty_move_counter(fifty_move_counter), 
                captured_piece(captured_piece) {}
        U64 hash_key;
        Castling castling_rights;
        Square enpass;
        I8 fifty_move_counter;
        Piece captured_piece;

    };
}