#include <iostream>

#include "defs.h"
#include "hashing/hash.h"
#include "move.h"
#include "search.h"
#include "types.h"

#include "tt.h"

namespace elixir {
    TranspositionTable tt[1];

    void TranspositionTable::clear_tt() {
        entries = 0;
        std::fill(table.begin(), table.end(), TTEntry());
    }

    void TranspositionTable::resize(U16 size) {
        U32 num_entries = (size * 0x100000 / sizeof(TTEntry)) - 2;
        table.resize(num_entries);
        clear_tt();
    }

    TranspositionTable::TranspositionTable(U16 size) {
        resize(size);
    }

    bool TranspositionTable::probe_tt(ProbedEntry &result, U64 key, U8 depth, int alpha, int beta,
                                      TTFlag &flag) {

        if (table.empty())
            return false;
        U32 index     = get_index(key);
        TTEntry entry = table[index];

        flag = TT_NONE;

        if (entry.key == key) {
            result.best_move = entry.move;
            result.score     = entry.score;
            result.depth     = entry.depth;
            flag             = entry.flag;
            return true;
        }
        return false;
    }

    void TranspositionTable::store_tt(U64 key, int score, move::Move move, U8 depth, int ply,
                                      TTFlag flag, search::PVariation pv) {
        U32 index     = get_index(key);
        TTEntry entry = table[index];

        bool replace = entry.key != key || entry.depth < depth + 2 || flag == TT_EXACT;

        if (! replace)
            return;

        if (entry.depth == -1)
            entries++;

        if (score > MATE)
            score += ply;
        else if (score < -MATE)
            score -= ply;

        entry.key   = key;
        entry.score = score;
        entry.move  = move;
        entry.depth = depth;
        entry.flag  = flag;

        table[index] = entry;
    }

}