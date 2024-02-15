#include <iostream>

#include "search.h"

#include "board/board.h"
#include "evaluate.h"
#include "move.h"
#include "movegen.h"
#include "utils/static_vector.h"
#include "utils/timer.h"

namespace elixir::search {
    int negamax(Board& board, int alpha, int beta, int depth, SearchInfo& info, PVariation& pv) {
        if (info.timed) {
            if (info.nodes % 2047 == 0) {
                if (timer::m_timer.time() - info.start_time > info.time_left) {
                    info.stopped = true;
                    return 0;
                }
            }
        }

        if (depth == 0) {
            return eval::evaluate(board);
        }

        auto local_pv = PVariation();
        int best_score = -50000;

        StaticVector<elixir::move::Move, 256> moves = movegen::generate_moves(board);

        for (const auto& move : moves) {
            if (!board.make_move(move)) { continue; }
            info.nodes++;
            info.ply++;
            int score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv);
            board.unmake_move(move, true);
            if (info.stopped) { return 0; }
            info.ply--;
            if (score > best_score) {
                best_score = score;
                if (score > alpha) {
                    alpha = score;
                    pv.load_from(move, local_pv);
                    pv.score = score;
                    if (score >= beta) {
                        return score;
                    }
                }
            }
        }

        return best_score;
    }
}