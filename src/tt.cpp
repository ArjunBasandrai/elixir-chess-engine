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

    bool TranspositionTable::probe_tt(ProbedEntry &result, U64 key, U8 depth, int alpha, int beta)
    {

        if (table.empty())
            return false;
        U32 index = get_index(key);
        TTEntry entry = table[index];

        if (entry.key == key)
        {
            result.best_move = entry.move;
            if (entry.depth > depth)
            {
                switch (entry.flag)
                {
                case TT_ALPHA:
                    if (entry.score <= alpha)
                    {
                        result = entry;
                        result.score = alpha;
                    }
                    else
                        return false;
                    break;
                case TT_BETA:
                    if (entry.score >= beta)
                    {
                        result = entry;
                        result.score = beta;
                    }
                    else
                        return false;
                    break;
                default:
                    result = entry;
                    break;
                }
                return true;
            }
        }
        return false;
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