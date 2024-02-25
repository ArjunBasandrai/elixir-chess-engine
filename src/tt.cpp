#include <iostream>

#include "defs.h"
#include "types.h"
#include "move.h"
#include "hashing/hash.h"

#include "tt.h"

namespace elixir
{
    void TranspositionTable::clear_tt()
    {
        std::fill(table.begin(), table.end(), TTEntry());
    }

    TranspositionTable::TranspositionTable(U16 size)
    {
        U32 num_entries = (size * 0x100000 / sizeof(TTEntry)) - 2;
        table.resize(num_entries);
        clear_tt();
    }

    ProbedEntry TranspositionTable::probe_tt(U64 key)
    {
        U32 index = get_index(key);
        TTEntry entry = table[index];
        if (entry.key == key)
        {
            ProbedEntry result;
            result.depth = entry.depth;
            result.score = entry.score;
            result.best_move = entry.move;
            result.flag = entry.flag;
            return result;
        }
        return ProbedEntry();
    }

    void TranspositionTable::store_tt(U64 key, int score, move::Move move, U8 depth, TTFlag flag)
    {
        U32 index = get_index(key);
        TTEntry entry = table[index];

        bool replace = entry.key == 0 || entry.depth + 2 < depth || flag == TT_EXACT;

        if (!replace)
            return;
        
        entry.key = key;
        entry.score = score;
        entry.move = move;
        entry.depth = depth;
        entry.flag = flag;  

        table[index] = entry; 
    }

}