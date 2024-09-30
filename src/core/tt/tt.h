#pragma once

#include <vector>

#include "defs.h"
#include "hashing/hash.h"
#include "move.h"
#include "search/pv.h"
#include "search/search.h"
#include "types.h"

namespace elixir {
    enum TTFlag : U8 { TT_NONE, TT_EXACT, TT_ALPHA, TT_BETA };

    struct TTEntry {
        U32 key         = 0ULL;
        I16 score       = 0;
        I16 static_eval = 0;
        move::Move move = move::NO_MOVE;
        I8 depth        = -1;
        TTFlag flag : 6 = TT_NONE;
        bool tt_pv : 2  = false;

        bool operator==(const TTEntry &other) const {
            return key == other.key && score == other.score && static_eval == other.static_eval &&
                   depth == other.depth && flag == other.flag;
        }
        bool operator==(const U64 &other) const { return key == other; }
    };

    struct ProbedEntry {
        I16 score;
        I16 static_eval;
        move::Move best_move;
        U8 depth;
        TTFlag flag : 6;
        bool tt_pv : 2;

        ProbedEntry()
            : score(0), static_eval(0), best_move(move::NO_MOVE), depth(0), flag(TT_NONE),
              tt_pv(false) {}

        ProbedEntry operator=(const TTEntry &entry) {
            score     = entry.score;
            best_move = entry.move;
            depth     = entry.depth;
            flag      = entry.flag;
            return *this;
        }

        bool is_usable_score(int alpha, int beta) const {
            return (flag == TT_EXACT || (flag == TT_ALPHA && score <= alpha) ||
                    (flag == TT_BETA && score >= beta));
        }
    };

    class TranspositionTable {
      public:
        TranspositionTable(U16 size = DEFAULT_HASH_SIZE);
        ~TranspositionTable() = default;
        void clear_tt();
        void resize(U16 size);
        void store_tt(U64 key, I16 score, I16 static_eval, move::Move move, U8 depth, int ply,
                      TTFlag flag, search::PVariation pv, bool tt_pv = false,
                      bool improving = false);
        bool probe_tt(ProbedEntry &result, U64 key, U8 depth, int alpha, int beta);
        int correct_score(int score, int ply) const {
            return (score > MATE_FOUND) ? score - ply : (score < -MATE_FOUND) ? score + ply : score;
        }
        U32 get_hashfull() {
            return static_cast<U32>(static_cast<F64>(entries) / static_cast<F64>(table.capacity()) *
                                    1000.0);
        }

        void prefetch(const U64 key) const {
            const U32 index = get_index(key);
            __builtin_prefetch(&table[index]);
        }

      private:
        U32 get_index(U64 key) const { return key % table.size(); }
        std::size_t entries;
        std::vector<TTEntry> table;
    };

    extern TranspositionTable tt[1];
}