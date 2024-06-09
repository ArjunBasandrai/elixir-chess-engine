#pragma once

#include <vector>

#include "defs.h"
#include "hashing/hash.h"
#include "move.h"
#include "search.h"
#include "types.h"

namespace elixir {
    enum TTFlag : U8 { TT_NONE, TT_EXACT, TT_ALPHA, TT_BETA };

    struct TTEntry {
        U64 key         = 0ULL;
        I16 score       = 0;
        move::Move move = move::NO_MOVE;
        I8 depth        = -1;
        TTFlag flag     = TT_NONE;

        bool operator==(const TTEntry &other) const {
            return key == other.key && score == other.score && depth == other.depth &&
                   flag == other.flag;
        }
        bool operator==(const U64 &other) const { return key == other; }
    };

    struct ProbedEntry {
        int score;
        move::Move best_move;
        U8 depth;
        TTFlag flag;

        ProbedEntry() : score(0), best_move(move::NO_MOVE), depth(0), flag(TT_NONE) {}

        ProbedEntry operator=(const TTEntry &entry) {
            score     = entry.score;
            best_move = entry.move;
            depth     = entry.depth;
            flag      = entry.flag;
            return *this;
        }
    };

    class TranspositionTable {
      public:
        TranspositionTable(U16 size = DEFAULT_HASH_SIZE);
        ~TranspositionTable() = default;
        void clear_tt();
        void resize(U16 size);
        void store_tt(U64 key, int score, move::Move move, U8 depth, int ply, TTFlag flag,
                      search::PVariation pv);
        bool probe_tt(ProbedEntry &result, U64 key, U8 depth, int alpha, int beta, TTFlag &flag);
        U32 get_hashfull() {
            return static_cast<U32>(static_cast<F64>(entries) / static_cast<F64>(table.capacity()) *
                                    1000.0);
        }

      private:
        U32 get_index(U64 key) const { return key % table.size(); }
        std::size_t entries;
        std::vector<TTEntry> table;
    };

    extern TranspositionTable tt[1];
}