#include <iostream>
#include <algorithm>

#include "search.h"

#include "board/board.h"
#include "evaluate.h"
#include "move.h"
#include "movepicker.h"
#include "movegen.h"
#include "tt.h"
#include "utils/static_vector.h"
#include "utils/timer.h"

namespace elixir::search
{
    void score_moves(Board &board, StaticVector<int, 256> &scores, MoveList &moves, move::Move tt_move, SearchStack *ss)
    {
        int value;
        Square from, to;
        int from_piece, to_piece;
        int from_val, to_val;
        for (int i = 0; i < moves.size(); i++)
        {

            move::Move move = moves[i];

            value = 0;

            // from Ethereal
            from = move.get_from();
            to = move.get_to();
            from_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(from)));
            to_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));
            from_val = eval::piece_values[from_piece];
            to_val = eval::piece_values[to_piece];

            if (move == tt_move)
            {
                // (~20 ELO)
                value += 16384;
            }

            if (move == ss->killers[0])
            {
                value += 512;
            }
            else if (move == ss->killers[1])
            {
                value += 256;
            }

            value += 5 * to_val;

            if (move.is_en_passant())
            {
                value += 2 * eval::piece_values[static_cast<int>(PieceType::PAWN)];
            }
            else if (move.is_promotion() && move.get_promotion() == move::Promotion::QUEEN)
            {
                value += 5 * eval::piece_values[static_cast<int>(PieceType::QUEEN)];
            }
            else if (move.is_castling())
            {
                value += 256;
            }
            scores[i] = value;
        }
    }

    // (~300 ELO)
    void sort_moves(Board &board, MoveList &moves, move::Move tt_move, SearchStack *ss)
    {
        StaticVector<int, 256> scores;
        scores.resize(moves.size());
        score_moves(board, scores, moves, tt_move, ss);

        for (int i = 0; i < moves.size(); i++)
        {
            for (int j = i + 1; j < moves.size(); j++)
            {
                if (scores[j] > scores[i])
                {
                    std::swap(scores[i], scores[j]);
                    std::swap(moves[i], moves[j]);
                }
            }
        }
    }

    // (~20 ELO)
    int qsearch(Board &board, int alpha, int beta, SearchInfo &info, PVariation &pv, SearchStack *ss)
    {

        pv.length = 0;

        if (info.timed) {
            if (info.nodes % 2047 == 0) {
                if (timer::m_timer.time() - info.start_time > info.time_left) {
                    info.stopped = true;
                    return 0;
                }
            }
        }

        int best_score, eval = eval::evaluate(board);

        if (ss->ply > MAX_PLY - 1) return eval;

        int legals = 0;
        auto local_pv = PVariation();
        auto best_move = move::Move();
        best_score = eval;

        ProbedEntry result;
        TTFlag flag = TT_ALPHA;
        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), 0, alpha, beta);
        const auto tt_move = tt_hit ? result.best_move : move::Move();

        if (tt_hit && ss->ply) {
            pv = result.pv;
            return result.score;
        }

        if (!board.is_in_check() && tt_hit)
        {
            best_score = result.score;
        }

        if (best_score >= beta)
        {
            return best_score;
        }

        alpha = std::max(alpha, best_score);

        MovePicker mp;
        mp.init_mp(board, tt_move, ss, true);
        move::Move move;

        while ((move = mp.next_move()) != move::NO_MOVE) {

            if (!board.make_move(move)) continue; 

            legals++;
            info.nodes++;

            int score = -qsearch(board, -beta, -alpha, info, local_pv, ss);
            board.unmake_move(move, true);

            if (info.stopped) return 0;

            if (score > best_score) {
                best_score = score;

                if (score > alpha) {
                    best_move = move;
                    alpha = score;
                    pv.load_from(move, local_pv);
                    pv.score = score;
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
        
        if (info.timed) {
            if (info.nodes % 2047 == 0) {
                if (timer::m_timer.time() - info.start_time > info.time_left) {
                    info.stopped = true;
                    return 0;
                }
            }
        }

        bool root_node = ss->ply == 0;
        bool pv_node = ((beta - alpha > 1) || root_node);
        bool in_check = board.is_in_check();

        // Check extension (~25 ELO)
        if (in_check) depth++;

        if (depth == 0) return qsearch(board, alpha, beta, info, pv, ss);
        
        if (ss->ply > MAX_PLY - 1) return board.get_eval();

        int legals = 0;

        auto local_pv = PVariation();
        int best_score = -INF;
        auto best_move = move::Move();
        ProbedEntry result;
        TTFlag flag = TT_ALPHA;

        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), depth, alpha, beta);
        // (~130 ELO)
        if (tt_hit && !pv_node) {
            pv = result.pv;
            return result.score;
        }

        const auto tt_move = tt_hit ? result.best_move : move::Move();
        
        MovePicker mp;
        mp.init_mp(board, tt_move, ss, false); 
        move::Move move;

        while ((move = mp.next_move()) != move::NO_MOVE) {

            if (!board.make_move(move)) continue;

            legals++;
            info.nodes++;

            const bool is_quiet_move = move.is_quiet();

            // PVS (~25 ELO)
            int score = 0;
            if (legals == 1) {
                score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv, ss + 1);
            } else {
                score = -negamax(board, -alpha - 1, -alpha, depth - 1, info, local_pv, ss + 1);
                if (score > alpha && score < beta) {
                    score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv, ss + 1);
                }
            }

            board.unmake_move(move, true);

            if (info.stopped) return 0;

            if (score > best_score) {
                best_move = move;
                best_score = score;
                if (score > alpha) {
                    pv.load_from(move, local_pv);
                    pv.score = score;
                    if (score >= beta) {
                        if (is_quiet_move) {
                            if (ss->killers[0] != move) {
                                ss->killers[1] = ss->killers[0];
                                ss->killers[0] = best_move;
                            }
                        }
                        flag = TT_BETA;
                        break;
                    }
                    flag = TT_EXACT;
                    alpha = score;
                }
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
            SearchStack stack[MAX_DEPTH], *ss = stack;
            for (int i = 0; i < MAX_DEPTH; i++) {
                stack[i].move = move::Move();
                stack[i].killers[0] = move::Move();
                stack[i].killers[1] = move::Move();
                stack[i].ply = i;
            }

            if (current_depth < 4) {
                score = negamax(board, alpha, beta, current_depth, info, pv, ss);
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

            if (print_info) {
                std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << info.nodes << " time " << duration.count() << " pv ";
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