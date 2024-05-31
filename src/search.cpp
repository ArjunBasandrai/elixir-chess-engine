#include <iostream>
#include <algorithm>
#include <cmath>

#include "search.h"

#include "board/board.h"
#include "evaluate.h"
#include "move.h"
#include "movepicker.h"
#include "movegen.h"
#include "tt.h"
#include "utils/static_vector.h"

namespace elixir::search
{
    int lmr[MAX_DEPTH][64] = {0};
    void init_lmr() {
        for (int depth = 1; depth < MAX_DEPTH; depth++) {
            for (int move = 1; move < 64; move++) {
                lmr[depth][move] = std::max(1.0, 0.75 + log(depth) * log(move) / 2.31);
            }
        }
    }

    bool should_stop(SearchInfo &info) {
        if (info.timed && !(info.nodes & 2047) &&
             std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - info.start_time).count() > info.time_left) {
                info.stopped = true;
                return true;
        }
        return false;
    }

    // (~20 ELO)
    int qsearch(Board &board, int alpha, int beta, SearchInfo &info, PVariation &pv, SearchStack *ss)
    {

        pv.length = 0;

        if (should_stop(info)) return 0;

        // Three-Fold Repetition Detection (~50 ELO)
        if (board.is_repetition()) return 0;

        int best_score, eval = eval::evaluate(board);

        if (ss->ply > MAX_DEPTH) return eval;

        int legals = 0;
        auto local_pv = PVariation();
        auto best_move = move::Move();
        best_score = eval;

        ProbedEntry result;
        TTFlag tt_flag = TT_NONE;
        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), 0, alpha, beta, tt_flag);
        const auto tt_move = result.best_move;

        bool can_cutoff = tt_hit && (tt_flag == TT_EXACT || (tt_flag == TT_ALPHA && result.score <= alpha) || (tt_flag == TT_BETA && result.score >= beta));

        if (ss->ply && can_cutoff) { return result.score; }

        if (best_score >= beta) { return best_score; }

        alpha = std::max(alpha, best_score);

        MovePicker mp;
        mp.init_mp(board, tt_move, ss, true);
        move::Move move;
        TTFlag flag = TT_ALPHA;

        while ((move = mp.next_move()) != move::NO_MOVE) {

            if (!board.make_move(move)) continue; 

            legals++;
            info.nodes++;

            int score = -qsearch(board, -beta, -alpha, info, local_pv, ss);
            board.unmake_move(move, true);

            if (info.stopped) return 0;

            if (score > best_score) {
                best_score = score;
                best_move = move;

                if (score > alpha) {
                    alpha = score;
                    pv.update(move, score, local_pv);
                    flag = TT_EXACT;
                }

                if (alpha >= beta) {
                    flag = TT_BETA;
                    break;
                }
            }
        }
        tt->store_tt(board.get_hash_key(), best_score, best_move, 0, ss->ply, flag, pv);
        return best_score;
    }

    int negamax(Board &board, int alpha, int beta, int depth, SearchInfo &info, PVariation &pv, SearchStack *ss) {
        
        pv.length = 0;
        
        if (should_stop(info)) return 0;

        bool root_node = ss->ply == 0;
        bool pv_node = ((beta - alpha > 1) || root_node);
        bool in_check = board.is_in_check();
        int eval;

        // Three-Fold Repetition Detection (~50 ELO)
        if (board.is_repetition()) return 0;

        // Check extension (~25 ELO)
        if (in_check) depth++;

        if (depth <= 0) return qsearch(board, alpha, beta, info, pv, ss);
        
        if (ss->ply > MAX_DEPTH) return board.get_eval();

        int legals = 0;

        auto local_pv = PVariation();
        int best_score = -INF;
        auto best_move = move::Move();
        ProbedEntry result;
        TTFlag tt_flag = TT_NONE;

        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), depth, alpha, beta, tt_flag);
        // TT Cutoff (~130 ELO)
        if (tt_hit && !pv_node && result.depth >= depth &&
            (tt_flag == TT_EXACT || (tt_flag == TT_ALPHA && result.score <= alpha) || (tt_flag == TT_BETA && result.score >= beta))) {
            return result.score;
        }

        const auto tt_move = result.best_move;

        // Internal Iterative Reduction (~6 ELO)
        if (depth >= 4 && tt_move == move::NO_MOVE) depth--;

        if (in_check) eval = ss->eval = INF;
        else eval = ss->eval = (tt_hit) ? result.score : eval::evaluate(board);

        if (!pv_node && !in_check) {
            // Razoring (~4 ELO)
            if (depth <= 5 && eval + 256 * depth < alpha) {
                const int razor_score = qsearch(board, alpha, beta, info, local_pv, ss);
                if (razor_score <= alpha) {
                    return razor_score;
                }
            }

            // Reverse Futility Pruning (~45 ELO)
            if (depth <= 6 && eval - 200 * depth >= beta) {
                return eval;
            }

            // Null Move Pruning (~60 ELO)
            if (depth >= 3 && (ss-1)->move != move::NO_MOVE && eval >= beta) {
                int R = 4 + depth / 6;
                R = std::min(R, depth);

                ss->move = move::NO_MOVE;

                board.make_null_move();
                int score = -negamax(board, -beta, -beta + 1, depth - R, info, local_pv, ss + 1);
                board.unmake_null_move();

                if (score >= beta) {
                    return beta;
                }
            }
        }

        MovePicker mp;
        mp.init_mp(board, tt_move, ss, false); 
        move::Move move;
        TTFlag flag = TT_ALPHA;

        MoveList bad_quiets;
        bool skip_quiets = false;

        while ((move = mp.next_move()) != move::NO_MOVE) {

            const bool is_quiet_move = move.is_quiet();

            if (skip_quiets && is_quiet_move) continue;
            
            if (!board.make_move(move)) continue;

            ss->move = move;

            legals++;
            info.nodes++;
            
            // Late Move Pruning [LMP]
            if (!root_node && best_score > -MATE_FOUND) {
                if (is_quiet_move && legals >= 8 + 3 * depth * depth) {
                    skip_quiets = true;
                }
            }

            // LMR + PVS (~40 ELO)
            int score = 0;
            if (legals == 1) {
                score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv, ss + 1);
            } else {
                int R = 1;
                if (is_quiet_move && depth >= 3 && legals > 1 + (pv_node ? 1 : 0)) {
                    R = lmr[std::min(63, depth)][std::min(63, legals)] + (pv_node ? 0 : 1);
                }
                score = -negamax(board, -alpha - 1, -alpha, depth - R, info, local_pv, ss + 1);
                if (score > alpha && (score < beta || R > 1)) {
                    score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv, ss + 1);
                }
            }

            board.unmake_move(move, true);

            if (info.stopped) return 0;

            if (score > best_score) {
                best_move = move;
                best_score = score;
                if (score > alpha) {
                    if (pv_node) pv.update(move, score, local_pv);
                    if (score >= beta) {
                        if (is_quiet_move) {
                            if (ss->killers[0] != move) {
                                ss->killers[1] = ss->killers[0];
                                ss->killers[0] = best_move;
                            }
                            board.update_history(move.get_from(), move.get_to(), depth, bad_quiets);
                        }
                        flag = TT_BETA;
                        break;
                    }
                    flag = TT_EXACT;
                    alpha = score;
                }
            }

            else if (is_quiet_move) {
                bad_quiets.push(move);
            }
        }

        if (legals == 0) {
            return board.is_in_check() ? -MATE + ss->ply : 0;
        }

        tt->store_tt(board.get_hash_key(), best_score, best_move, depth, ss->ply, flag, pv);

        return best_score;
    }

    void search(Board &board, SearchInfo &info, bool print_info) {
        auto start = std::chrono::high_resolution_clock::now();
        PVariation pv;
        for (int current_depth = 1; current_depth <= info.depth; current_depth++) {
            int score = 0, alpha = -INF, beta = INF, delta = 10;
            SearchStack stack[MAX_DEPTH + 4], *ss = stack + 4;
            for (int i = -4; i < MAX_DEPTH; i++) {
                (ss+i)->move = move::NO_MOVE;
                (ss+i)->killers[0] = move::NO_MOVE;
                (ss+i)->killers[1] = move::NO_MOVE;
                (ss+i)->eval = INF;
            }

            for (int i = 0; i < MAX_DEPTH; i++) {
                (ss+i)->ply = i;
            }

            if (info.depth >= 4) {
                alpha = std::max(-INF, score - delta);
                beta = std::min(INF, score + delta);
            }

            // aspiration windows
            while (1) {
                score = negamax(board, alpha, beta, current_depth, info, pv, ss);

                if (score > alpha && score < beta) break;

                if (score <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(-INF, alpha - delta);
                }

                else if (score >= beta) {
                    beta = std::min(INF, beta + delta);
                }

                delta = delta + delta / 2;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if (info.stopped) break;

            if (print_info) {
                if (score > -MATE && score < -MATE_FOUND) {
                    std::cout << "info score mate " << -(score + MATE) / 2 << " depth " << current_depth << " nodes " << info.nodes << " time " << duration.count() << " pv ";
                }

                else if (score > MATE_FOUND && score < MATE) {
                    std::cout << "info score mate " << (MATE - score) / 2 + 1 << " depth " << current_depth << " nodes " << info.nodes << " time " << duration.count() << " pv ";
                } 

                else {
                    std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << info.nodes << " time " << duration.count() << " pv ";
                }
                pv.print_pv();
                std::cout << std::endl;
            }
        }

        if (print_info) {
            std::cout << "bestmove ";
            pv.line[0].print_uci();
            std::cout << std::endl;
        }
    }
}