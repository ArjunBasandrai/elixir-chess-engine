#include <algorithm>
#include <cmath>
#include <iostream>

#include "search.h"
#include "search_terms.h"
#include "ss.h"

#include "board/board.h"
#include "evaluation/evaluate.h"
#include "history/history.h"
#include "move.h"
#include "movegen.h"
#include "movepicker.h"
#include "time_manager/time_manager.h"
#include "tt/tt.h"
#include "utils/bits.h"
#include "utils/static_vector.h"

using namespace elixir::bits;

namespace elixir::search {
    int lmr[MAX_DEPTH][64] = {0};
    void init_lmr() {
        for (int depth = 0; depth < MAX_DEPTH; depth++) {
            for (int move = 0; move < 64; move++) {
                if (move == 0 || depth == 0) {
                    lmr[depth][move] = 0;
                    continue;
                }
                lmr[depth][move] = std::max(1.0, LMR_OFFSET + log(depth) * log(move) / LMR_DIVISOR);
            }
        }
    }

    void Searcher::update_killers_and_histories(SearchStack *ss, move::Move move,
                                                MoveList &bad_quiets, Color stm, int depth,
                                                bool is_quiet_move) {
        if (is_quiet_move) {
            if (ss->killers[0] != move) {
                ss->killers[1] = ss->killers[0];
                ss->killers[0] = move;
            }
            history.countermove_history.update_countermove(stm, (ss - 1)->move.get_from(),
                                                           (ss - 1)->move.get_to(), move);
            history.quiet_history.update_history(move.get_from(), move.get_to(), depth, bad_quiets);
            history.continuation_history.update_chs(move, ss, bad_quiets, depth);
        }
    }

    // (~20 ELO)
    int Searcher::qsearch(ThreadData &td, int alpha, int beta, PVariation &pv, SearchStack *ss) {

        pv.length = 0;

        auto &board = td.board;
        auto &info  = td.info;

        if (time_manager.should_stop(info) || info.stopped)
            return 0;

        if (ss->ply > info.seldepth)
            info.seldepth = ss->ply;

        // Three-Fold Repetition Detection (~50 ELO)
        if (board.is_repetition() || board.is_material_draw())
            return 0;

        if (ss->ply >= MAX_DEPTH - 1)
            return board.evaluate();

        ProbedEntry result;
        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), 0, alpha, beta);

        TTFlag tt_flag     = result.flag;
        const auto tt_move = result.best_move;

        bool usable_tt_score = tt_hit && result.is_usable_score(alpha, beta);

        int eval            = (tt_hit) ? result.static_eval : board.evaluate();
        I16 raw_static_eval = eval;

        /*
        | If TT score is found and it is usable, then cutoff. |
        */
        if (ss->ply && usable_tt_score) {
            return tt->correct_score(result.score, ss->ply);
        }

        auto local_pv  = PVariation();
        auto best_move = move::Move();
        int best_score = eval;
        /*
        | If TT score is found and it is usable, then use it. |
        */
        if (tt_hit && usable_tt_score) {
            best_score = result.score;
        }

        if (best_score >= beta) {
            return best_score;
        }

        alpha = std::max(alpha, best_score);

        MovePicker mp;
        mp.init_mp(board, tt_move, ss, history, true);
        move::Move move;
        TTFlag flag = TT_ALPHA;

        if (! ss->ply)
            info.best_root_move = mp.first_move();

        while ((move = mp.next_move())) {

            /*
            | Q-Search Static Exchange Evaluation [SEE] Pruning (~55 ELO) : Skip moves that |
            | lose a lot of material.                                                       |
            */
            if (! SEE(board, move, -QS_SEE_THRESHOLD))
                continue;

            /*
            | If current move is illegal, then skip it. |
            */
            if (! board.make_move(move))
                continue;

            /*
            | Prefetch TT entry for current position. |
            */
            tt->prefetch(board.get_hash_key());

            info.nodes++;

            int score = -qsearch(td, -beta, -alpha, local_pv, ss);
            board.unmake_move(move, true);

            if (info.stopped)
                return 0;

            if (score > best_score) {
                best_score = score;
                if (score > alpha) {
                    best_move  = move;
                    pv.update(move, local_pv);
                    if (score >= beta) {
                        flag = TT_BETA;
                        break;
                    }
                    alpha = score;
                    flag = TT_ALPHA;
                }

            }
        }
        tt->store_tt(board.get_hash_key(), best_score, raw_static_eval, best_move, 0, ss->ply, flag,
                     pv);
        return best_score;
    }

    int Searcher::negamax(ThreadData &td, int alpha, int beta, int depth, PVariation &pv,
                          SearchStack *ss, bool cutnode) {

        pv.length = 0;

        auto &board = td.board;
        auto &info  = td.info;

        bool root_node = ss->ply == 0;
        bool pv_node   = ((beta - alpha > 1) || root_node);
        bool in_check  = board.is_in_check();

        if (! root_node && (time_manager.should_stop(info) || info.stopped))
            return 0;

        if (ss->ply > info.seldepth)
            info.seldepth = ss->ply;

        /*
        | Quiescence Search : Perform a quiescence search at leaf nodes to avoid the horizon effect.
        |
        */
        if (depth <= 0)
            return qsearch(td, alpha, beta, pv, ss);

        if (! root_node) {
            /*
            | 3-Fold Repetition Detection (~50 ELO) : If the position has been repeated 3 times, |
            | then the position is a draw.                                                       |
            */
            if (board.is_repetition() || board.is_material_draw())
                return 0;

            if (ss->ply >= MAX_DEPTH - 1)
                return board.evaluate();

            /*
            | Mate Distance Pruning : If a forced mate is already found, cut the search and adjust |
            | the bounds if no shorter mate is possible.                                           |
            */
            alpha = std::max(alpha, -MATE + ss->ply);
            beta  = std::min(beta, MATE - ss->ply - 1);
            if (alpha >= beta)
                return alpha;
        }

        int legals = 0;

        auto local_pv  = PVariation();
        int best_score = -INF;
        auto best_move = move::Move();

        ProbedEntry result;

        TTFlag tt_flag;
        bool tt_hit           = false;
        bool can_use_tt_score = false;
        auto tt_move          = move::NO_MOVE;

        if (! ss->excluded_move) {
            tt_hit  = tt->probe_tt(result, board.get_hash_key(), depth, alpha, beta);
            tt_flag = result.flag;
            /*
            | Check to if TT score is usable. |
            */
            can_use_tt_score = result.is_usable_score(alpha, beta);
            /*
            | TT Cutoff (~130 ELO) : If we have already seen this position before and the |
            | stored score is useful, we can use the previously stored score to avoid     |
            | searching the same position again.                                          |
            */
            if (tt_hit && ! pv_node && result.depth >= depth && can_use_tt_score) {
                return tt->correct_score(result.score, ss->ply);
            }
            tt_move = result.best_move;
        }

        const bool tt_pv = (tt_hit && result.tt_pv) || pv_node;

        /*
        | Internal Iterative Reduction (~6 ELO) : If no TT move is found for this position, |
        | searching this node will likely take a lot of time, and this node is likely to be |
        | not very good. So, we save time by reducing the depth of the search.              |
        */
        if (depth >= IIR_DEPTH && ! tt_move)
            depth--;

        /*
        | Initialize the evaluation score. If we are in check, we set the evaluation score to INF. |
        | Otherwise, if we have a TT hit, we use the stored score. If not, we evaluate the position.
        |
        */
        if (! ss->excluded_move) {
            ss->static_eval = (tt_hit) ? result.static_eval : board.evaluate();
            if (in_check)
                ss->eval = SCORE_NONE;

            else {
                ss->eval = (tt_hit && can_use_tt_score) ? result.score : ss->static_eval;
            }
        }

        I16 raw_static_eval = ss->static_eval;

        /*
        | Improving Heuristic (~10 ELO) : Check if our position is better than it was 2 or 4 plies
        before. |
        */
        const bool improving = [&] {
            if (in_check)
                return false;
            if ((ss - 2)->static_eval != SCORE_NONE)
                return ss->static_eval > (ss - 2)->static_eval;
            if ((ss - 4)->static_eval != SCORE_NONE)
                return ss->static_eval > (ss - 4)->static_eval;
            return true;
        }();

        if (! pv_node && ! in_check && ! ss->excluded_move) {
            /*
            | Razoring (~4 ELO) : If out position is way below alpha, do a verification |
            | quiescence search, if we still cant exceed alpha, then we cutoff.         |
            */
            if (depth <= RAZOR_DEPTH && ss->eval + RAZOR_MARGIN * depth < alpha) {
                const int razor_score = qsearch(td, alpha, beta, local_pv, ss);
                if (razor_score <= alpha) {
                    return razor_score;
                }
            }

            /*
            | Reverse Futility Pruning (~45 ELO) : If our position is so good, that we are |
            | confident that we will not fall below beta anytime soon, then we cutoff.     |
            */
            if (depth <= RFP_DEPTH &&
                ss->eval - futility_margin(depth, improving, cutnode, tt_hit) >= beta &&
                beta >= -MATE_FOUND && ss->eval <= MATE_FOUND) {
                return (ss->eval + beta) / 2;
            }

            /*
            | Null Move Pruning (~60 ELO) : If our position is so good, we give our |
            | opponent an extra move to see if we are still better.                 |
            */
            if (depth >= NMP_DEPTH && (ss - 1)->move && ss->eval >= beta &&
                ss->static_eval >= beta + 170 - 24 * depth && board.has_non_pawn_material()) {
                int R = NMP_BASE_REDUCTION + depth / NMP_DIVISOR +
                        std::min<double>((ss->eval - beta) / NMP_EVAL_BASE, (NMP_EVAL_MAX / 10.0)) +
                        std::min(board.get_phase(), NMP_PHASE_MAX) / (NMP_PHASE_BASE / 10.0);
                R = std::min(R, depth);

                /*
                | Set current move to a null move in the search stack to avoid |
                | multiple null move searching in a row.                       |
                */
                ss->move      = move::NO_MOVE;
                ss->cont_hist = nullptr;

                board.make_null_move();
                int score = -negamax(td, -beta, -beta + 1, depth - R, local_pv, ss + 1, ! cutnode);
                board.unmake_null_move();

                /*
                | If our position is still better even after giving our opponent |
                | an extra move, we can safely assume that we are winning.       |
                */
                if (score >= beta) {
                    return beta;
                }
            }
        }

        /*
        | Initialize MovePicker, generate all moves and use TT Move for move ordering. |
        */
        MovePicker mp;
        mp.init_mp(board, tt_move, ss, history, false);
        if (root_node)
            info.best_root_move = mp.first_move();

        TTFlag flag = TT_ALPHA;
        move::Move move;
        bool skip_quiets = false;

        /*
        | Initialize a bad quiets array to be used by history maluses. |
        */
        MoveList bad_quiets;

        while ((move = mp.next_move())) {

            if (move == ss->excluded_move)
                continue;

            const bool is_quiet_move = move.is_quiet();

            /*
            | If skip_quiets is enabled by Late Move Pruning, we skip quiet |
            | moves that are likely to be bad.                              |
            */
            if (skip_quiets && is_quiet_move)
                continue;

            if (! root_node && best_score > -MATE_FOUND) {
                /*
                | Late Move Pruning [LMP] (~30 ELO) : Skip late quiet moves if  |
                | we've already searched the most promising moves because they  |
                | are likely to be bad.                                         |
                */
                if (is_quiet_move &&
                    legals >= LMP_BASE + LMP_MULTIPLIER * depth * depth / (2 - improving) &&
                    board.has_non_pawn_material()) {
                    skip_quiets = true;
                    continue;
                }

                /*
                | Futility Pruning (~5 ELO) : Skip futile quiet moves at near-leaf nodes |
                | when there's a little chance of improving alpha.                       |
                */
                const int futility_margin = FP_BASE + FP_MULTIPLIER * depth;
                if (depth <= FP_DEPTH && ! in_check && is_quiet_move &&
                    ss->static_eval + futility_margin < alpha) {
                    skip_quiets = true;
                    continue;
                }

                /*
                | Static Exchange Evaluation [SEE] Pruning (~20 ELO) : Skip moves that |
                | lose a lot a material.                                               |
                */
                const int see_threshold =
                    is_quiet_move ? -SEE_QUIET * depth : -SEE_CAPTURE * depth * depth;
                if (depth <= SEE_DEPTH && legals > 0 && ! SEE(board, move, see_threshold))
                    continue;
            }

            int extensions = 0;

            /*
            | Singular Extensions (~40 ELO) : If a TT move exists and the score is usable, |
            | perform a reduced depth search with TT move excluded to see if any other     |
            | move can beat it or not (in other words if TT move is singular or not).      |
            */
            if (! root_node && depth >= 8 && move == tt_move && ! ss->excluded_move &&
                result.depth >= depth - 3 && tt_flag != TT_ALPHA) {
                const auto s_beta  = result.score - depth * (SE_BETA_MULTIPLIER / 100.0);
                const auto s_depth = (depth - 1) / 2;

                ss->excluded_move = move;

                const int s_score = negamax(td, s_beta - 1, s_beta, s_depth, local_pv, ss, cutnode);

                ss->excluded_move = move::NO_MOVE;

                /*
                | TT move is singular, so we extend it's search because it is likely to be very
                good. |
                */
                if (s_score < s_beta) {
                    /*
                    | Double Extensions (~2 ELO) : Double extend the TT move's search |
                    | if it is better by a certain margin.                            |
                    */
                    const int double_margin = DOUBLE_EXT_MARGIN * pv_node;
                    extensions += 1 + (s_score < s_beta - double_margin);
                }

                /*
                | Multicut (~9 ELO) : If singular search had a beta cutoff, this indicates   |
                | that TT move was no singular. So, we prune if ti can cause a cutoff again. |
                */
                else if (s_beta >= beta)
                    return s_beta;

                /*
                | Negative Extensions (~9 ELO) : If the TT move was not singular, search it |
                | with reduced depth, as it can cause a beta cutoff again.                  |
                */
                else if (result.score >= beta)
                    extensions--;

                else if (cutnode)
                    extensions--;
            }

            /*
            | Check Extension (~20 ELO) : Extend the search if current position is in check. |
            */
            extensions += in_check;

            /*
            | If current move is illegal, then skip it. |
            */
            if (! board.make_move(move))
                continue;

            /*
            | Prefetch TT entry for current position. |
            */
            tt->prefetch(board.get_hash_key());

            /*
            | Add the current move to search stack. |
            */
            ss->move = move;
            /*
            | Fetch correct continuation history entry for the position. |
            */
            ss->cont_hist = history.continuation_history.get_cont_hist_entry(move);

            legals++;
            info.nodes++;

            const int history_score = history.get_history(move, ss);

            /*
            | Principal Variation Search and Late Move Reduction [PVS + LMR] (~40 ELO) |
            */
            int score           = 0;
            const int new_depth = depth - 1 + extensions;

            int R = lmr[std::min(63, depth)][std::min(63, legals)];
            R += ! pv_node;
            R -= (is_quiet_move ? history_score / LMR_HISTORY_BASE : 0);
            R -= board.is_in_check();
            R += cutnode;
            R -= tt_pv;
            R += ! improving;

            if (depth > 1 && legals > 1) {
                R             = std::clamp(R, 1, new_depth);
                int lmr_depth = new_depth - R + 1;
                score         = -negamax(td, -alpha - 1, -alpha, lmr_depth, local_pv, ss + 1, true);

                if (score > alpha && R > 0) {
                    score =
                        -negamax(td, -alpha - 1, -alpha, new_depth, local_pv, ss + 1, ! cutnode);
                }
            }

            else if (! pv_node || legals > 1) {
                score = -negamax(td, -alpha - 1, -alpha, new_depth, local_pv, ss + 1, ! cutnode);
            }

            if (pv_node && (legals == 1 || (score > alpha && (root_node || score < beta)))) {
                score = -negamax(td, -beta, -alpha, new_depth, local_pv, ss + 1, false);
            }

            board.unmake_move(move, true);

            if (info.stopped)
                return 0;

            if (score > best_score) {
                best_score = score;
                if (score > alpha) {
                    best_move  = move;
                    if (pv_node)
                        pv.update(move, local_pv);
                    if (score >= beta) {
                        update_killers_and_histories(ss, move, bad_quiets, board.get_side_to_move(),
                                                     depth, is_quiet_move);
                        flag = TT_BETA;
                        break;
                    }
                    flag  = TT_EXACT;
                    alpha = score;
                }
            }

            else if (is_quiet_move) {
                bad_quiets.push(move);
            }
        }

        if (legals == 0) {
            if (ss->excluded_move)
                return alpha;
            return board.is_in_check() ? -MATE + ss->ply : 0;
        }

        if (! ss->excluded_move) {
            tt->store_tt(board.get_hash_key(), best_score, raw_static_eval, best_move, depth,
                         ss->ply, flag, pv, tt_pv, improving);
        }

        return best_score;
    }

    bool SEE(const Board &board, const move::Move move, int threshold, const int see_values[7]) {

        if (move.is_promotion())
            return true;

        Square from = move.get_from();
        Square to   = move.get_to();

        int target_piece =
            move.is_en_passant() ? 0 : static_cast<int>(piece_to_piecetype(board.piece_on(to)));


        int value = see_values[target_piece] - threshold;

        if (value < 0)
            return false;

        int attacker_piece = static_cast<int>(piece_to_piecetype(board.piece_on(from)));

        value -= see_values[attacker_piece];
        if (value >= 0)
            return true;

        Square sq =
            move.is_en_passant()
                ? static_cast<Square>(static_cast<int>(to) -
                                      8 * color_offset[static_cast<int>(board.get_side_to_move())])
                : to;
        Bitboard occupied  = board.occupancy() ^ bit(from) ^ bit(sq);
        Bitboard attackers = board.get_attackers(to, Color::WHITE, occupied) |
                             board.get_attackers(to, Color::BLACK, occupied);

        Bitboard bishops = board.bishops() | board.queens();
        Bitboard rooks   = board.rooks() | board.queens();

        Color side = piece_color(board.piece_on(from));
        side       = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;

        while (true) {
            attackers &= occupied;

            Bitboard my_attackers = attackers & board.color_occupancy(side);
            if (! my_attackers)
                break;

            // Pick least valuable attacker
            int piece;
            for (piece = 0; piece < 6; piece++) {
                if (my_attackers & board.piece_bitboard(static_cast<PieceType>(piece))) {
                    break;
                }
            }

            side = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;

            value = -value - 1 - see_values[piece];
            if (value >= 0) {
                if (piece == 5 && (attackers & board.color_occupancy(side))) {
                    side = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
                }
                break;
            }

            Color enemy_side = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
            occupied ^= bit(static_cast<Square>(
                lsb_index(my_attackers & board.piece_bitboard(static_cast<PieceType>(piece)))));

            if (piece == 0 || piece == 2 || piece == 4) {
                attackers |= attacks::get_bishop_attacks(to, occupied) & bishops;
            }
            if (piece == 3 || piece == 4) {
                attackers |= attacks::get_rook_attacks(to, occupied) & rooks;
            }
        }

        return side != piece_color(board.piece_on(from));
    }

    void Searcher::search(ThreadData &td, bool print_info) {
        print_info &= td.thread_idx == 0;
        searching  = true;
        auto start = std::chrono::high_resolution_clock::now();
        PVariation pv;
        move::Move best_move;

        auto &info = td.info;

        for (int current_depth = 1; current_depth <= info.depth; current_depth++) {
            info.seldepth = 0;
            int score = 0, alpha = -INF, beta = INF, delta = INITIAL_ASP_DELTA;
            SearchStack stack[MAX_DEPTH + 4], *ss = stack + 4;
            for (int i = -4; i < MAX_DEPTH; i++) {
                (ss + i)->move        = move::NO_MOVE;
                (ss + i)->killers[0]  = move::NO_MOVE;
                (ss + i)->killers[1]  = move::NO_MOVE;
                (ss + i)->eval        = SCORE_NONE;
                (ss + i)->static_eval = SCORE_NONE;
            }

            for (int i = 0; i < MAX_DEPTH; i++) {
                (ss + i)->ply = i;
            }

            if (info.depth >= MIN_ASP_DEPTH) {
                alpha = std::max(-INF, score - delta);
                beta  = std::min(INF, score + delta);
            }

            // aspiration windows
            while (1) {
                score = negamax(td, alpha, beta, current_depth, pv, ss, false);

                if (score > alpha && score < beta)
                    break;

                if (score <= alpha) {
                    beta  = (alpha + beta) / 2;
                    alpha = std::max(-INF, alpha - delta);
                }

                else if (score >= beta) {
                    beta = std::min(INF, beta + delta);
                }

                delta *= ASP_MULTIPLIER;

                if (time_manager.should_stop(info) || info.stopped)
                    break;
            }

            auto end      = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            best_move = (pv.line[0]) ? pv.line[0] : info.best_root_move;

            print_info &= bool(pv.line[0]);

            if (! soft_stop && print_info) {
                std::cout << "info string depth " << current_depth << " unfinished\n" << std::endl;
            } else if (print_info) {
                int time_ms = duration.count();
                int nps     = info.nodes * 1000 / (time_ms + 1);
                if (score > -MATE && score < -MATE_FOUND) {
                    std::cout << "info score mate " << -(score + MATE) / 2 << " depth "
                              << current_depth << " seldepth " << info.seldepth << " nodes "
                              << main_searcher.get_nodes() << " time " << time_ms << " nps " << nps
                              << " hashfull " << tt->get_hashfull() << " pv ";
                }

                else if (score > MATE_FOUND && score < MATE) {
                    std::cout << "info score mate " << (MATE - score) / 2 + 1 << " depth "
                              << current_depth << " seldepth " << info.seldepth << " nodes "
                              << main_searcher.get_nodes() << " time " << time_ms << " nps " << nps
                              << " hashfull " << tt->get_hashfull() << " pv ";
                }

                else {
                    std::cout << "info score cp " << score << " depth " << current_depth
                              << " seldepth " << info.seldepth << " nodes "
                              << main_searcher.get_nodes() << " time " << time_ms << " nps " << nps
                              << " hashfull " << tt->get_hashfull() << " pv ";
                }
                pv.print_pv();
                std::cout << std::endl;
            }

            if (time_manager.should_stop_early(info, current_depth, best_move) || info.stopped)
                break;
        }

        if (print_info) {
            std::cout << "bestmove ";
            best_move.print_uci();
            std::cout << std::endl;
        }

        searching = false;
    }

    void ThreadManager::search(Board &board, SearchInfo &info, bool print_info) {
        in_search                = true;
        SearchInfo non_main_info = SearchInfo(MAX_DEPTH, false);
        for (int i = 0; i < num_threads; i++) {
            thread_datas.push_back(ThreadData(board, i ? non_main_info : info));
            thread_datas[i].thread_idx = i;
        }

        for (int i = 1; i < num_threads; i++) {
            auto &td       = thread_datas[i];
            auto &searcher = searchers[i];
            threads.emplace_back(
                std::thread([&td, &searcher, print_info]() { searcher.search(td, print_info); }));
        }

        searchers[0].search(thread_datas[0], print_info);

        for (int i = 1; i < num_threads; i++) {
            if (searchers[i].searching)
                thread_datas[i].info.stopped = true;
        }

        for (auto &thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        for (auto &td : thread_datas) {
            info.nodes += td.info.nodes;
        }

        thread_datas.clear();
        threads.clear();

        in_search = false;
    }

}