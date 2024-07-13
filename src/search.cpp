#include <algorithm>
#include <cmath>
#include <iostream>

#include "search.h"
#include "ss.h"

#include "board/board.h"
#include "evaluate.h"
#include "move.h"
#include "movegen.h"
#include "movepicker.h"
#include "tt.h"
#include "time_manager.h"
#include "history.h"
#include "utils/bits.h"
#include "utils/static_vector.h"

using namespace elixir::bits;

namespace elixir::search {
    int LMP_BASE           = 2;
    int RFP_MARGIN         = 72;
    int RAZOR_MARGIN       = 298;
    int NMP_BASE_REDUCTION = 5;
    int NMP_DEPTH          = 1;
    int RFP_DEPTH          = 6;
    int RAZOR_DEPTH        = 7;
    int IIR_DEPTH          = 4;
    int LMP_MULTIPLIER     = 1;
    int LMR_DEPTH          = 4;
    int FP_BASE            = 146;
    int FP_MULTIPLIER      = 118;
    int FP_DEPTH           = 5;
    int SEE_QUIET          = 67;
    int SEE_CAPTURE        = 32;
    int SEE_DEPTH          = 10;
    int QS_SEE_THRESHOLD   = 9;
    int SEE_PAWN           = 77;
    int SEE_KNIGHT         = 299;
    int SEE_BISHOP         = 303;
    int SEE_ROOK           = 504;
    int SEE_QUEEN          = 903;
    int INITIAL_ASP_DELTA  = 12;
    int NMP_DIVISOR        = 9;
    double LMR_OFFSET      = 0.5137;
    double LMR_DIVISOR     = 1.711;
    int MIN_ASP_DEPTH      = 4;
    double ASP_MULTIPLIER  = 1.3111;
}

namespace elixir::search {

    int see_pieces[7] = {SEE_PAWN, SEE_KNIGHT, SEE_BISHOP, SEE_ROOK, SEE_QUEEN, 0, 0};

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

    // (~20 ELO)
    int qsearch(Board &board, int alpha, int beta, SearchInfo &info, PVariation &pv,
                SearchStack *ss) {

        pv.length = 0;

        if (time_manager.should_stop(info) || info.stopped)
            return 0;

        if (ss->ply > info.seldepth)
            info.seldepth = ss->ply;

        // Three-Fold Repetition Detection (~50 ELO)
        if (board.is_repetition() || board.is_material_draw())
            return 0;

        int best_score, eval = eval::evaluate(board);

        if (ss->ply >= MAX_DEPTH - 1)
            return eval;

        int legals     = 0;
        auto local_pv  = PVariation();
        auto best_move = move::Move();
        best_score     = eval;

        ProbedEntry result;
        TTFlag tt_flag     = TT_NONE;
        const bool tt_hit  = tt->probe_tt(result, board.get_hash_key(), 0, alpha, beta, tt_flag);
        const auto tt_move = result.best_move;

        bool can_cutoff =
            tt_hit && (tt_flag == TT_EXACT || (tt_flag == TT_ALPHA && result.score <= alpha) ||
                       (tt_flag == TT_BETA && result.score >= beta));

        if (ss->ply && can_cutoff) {
            return result.score;
        }

        if (best_score >= beta) {
            return best_score;
        }

        alpha = std::max(alpha, best_score);

        MovePicker mp;
        mp.init_mp(board, tt_move, ss, true);
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

            if (! board.make_move(move))
                continue;

            legals++;
            info.nodes++;

            int score = -qsearch(board, -beta, -alpha, info, local_pv, ss);
            board.unmake_move(move, true);

            if (info.stopped)
                return 0;

            if (score > best_score) {
                best_score = score;
                best_move  = move;

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

    int negamax(Board &board, int alpha, int beta, int depth, SearchInfo &info, PVariation &pv,
                SearchStack *ss) {

        pv.length = 0;

        bool root_node = ss->ply == 0;
        bool pv_node   = ((beta - alpha > 1) || root_node);
        bool in_check  = board.is_in_check();
        int eval;

        if (! root_node && (time_manager.should_stop(info) || info.stopped))
            return 0;

        if (ss->ply > info.seldepth)
            info.seldepth = ss->ply;

        /*
        | Check Extension (~25 ELO) : If we are in check, extend the search depth and avoid dropping |
        | to qsearch.                                                                                |
        */
        if (in_check)
            depth++;

        /*
        | Quiescence Search : Perform a quiescence search at leaf nodes to avoid the horizon effect. |
        */
        if (depth <= 0)
            return qsearch(board, alpha, beta, info, pv, ss);


        if (! root_node) {
            /*
            | 3-Fold Repetition Detection (~50 ELO) : If the position has been repeated 3 times, |
            | then the position is a draw.                                                       |
            */
            if (board.is_repetition() || board.is_material_draw())
                return 0;

            if (ss->ply >= MAX_DEPTH - 1)
                return eval::evaluate(board);

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
        TTFlag tt_flag = TT_NONE;

        const bool tt_hit = tt->probe_tt(result, board.get_hash_key(), depth, alpha, beta, tt_flag);
        /*
        | TT Cutoff (~130 ELO) : If we have already seen this position before and the |
        | stored score is useful, we can use the previously stored score to avoid     |
        | searching the same position again.                                          |
        */
        if (tt_hit && ! pv_node && result.depth >= depth &&
            (tt_flag == TT_EXACT || (tt_flag == TT_ALPHA && result.score <= alpha) ||
             (tt_flag == TT_BETA && result.score >= beta))) {
            return result.score;
        }

        /*
        | TT Move : Use the stored move from the transposition table for move ordering. |
        */
        const auto tt_move = result.best_move;

        /*
        | Internal Iterative Reduction (~6 ELO) : If no TT move is found for this position, |
        | searching this node will likely take a lot of time, and this node is likely to be |
        | not very good. So, we save time by reducing the depth of the search.              |
        */
        if (depth >= IIR_DEPTH && !tt_move)
            depth--;

        /*
        | Initialize the evaluation score. If we are in check, we set the evaluation score to INF.   |
        | Otherwise, if we have a TT hit, we use the stored score. If not, we evaluate the position. |
        */
        if (in_check)
            eval = ss->eval = INF;

        else
            eval = ss->eval = (tt_hit) ? result.score : eval::evaluate(board);

        const bool improving = [&] {
            if (in_check)
                return false;
            if (ss->ply > 1 && (ss - 2)->eval != -INF)
                return ss->eval > (ss - 2)->eval;
            if (ss->ply > 3 && (ss - 4)->eval != -INF)
                return ss->eval > (ss - 4)->eval;
            return true;
        }();

        if (! pv_node && ! in_check) {
            /*
            | Razoring (~4 ELO) : If out position is way below alpha, do a verification |
            | quiescence search, if we still cant exceed alpha, then we cutoff.         |
            */
            if (depth <= RAZOR_DEPTH && eval + RAZOR_MARGIN * depth < alpha) {
                const int razor_score = qsearch(board, alpha, beta, info, local_pv, ss);
                if (razor_score <= alpha) {
                    return razor_score;
                }
            }
            /*
            | Reverse Futility Pruning (~45 ELO) : If our position is so good, that we are |
            | confident that we will not fall below beta anytime soon, then we cutoff.     |
            */
            if (depth <= RFP_DEPTH && eval - RFP_MARGIN * depth + 60 * improving >= beta) {
                return eval;
            }

            /*
            | Null Move Pruning (~60 ELO) : If our position is so good, we give our |
            | opponent an extra move to see if we are still better.                 |
            */
            if (depth >= NMP_DEPTH && (ss - 1)->move && eval >= beta) {
                int R = NMP_BASE_REDUCTION + depth / NMP_DIVISOR;
                R     = std::min(R, depth);

                /*
                | Set current move to a null move in the search stack to avoid |
                | multiple null move searching in a row.                       |
                */
                ss->move      = move::NO_MOVE;
                ss->cont_hist = nullptr;

                board.make_null_move();
                int score = -negamax(board, -beta, -beta + 1, depth - R, info, local_pv, ss + 1);
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
        mp.init_mp(board, tt_move, ss, false);
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
                if (is_quiet_move && legals >= LMP_BASE + LMP_MULTIPLIER * depth * depth) {
                    skip_quiets = true;
                    continue;
                }

                /*
                | Futility Pruning (~5 ELO) : Skip futile quiet moves at near-leaf nodes |
                | when there's a little chance of improving alpha.                       |
                */
                const int futility_margin = FP_BASE + FP_MULTIPLIER * depth;
                if (depth <= FP_DEPTH && ! in_check && is_quiet_move &&
                    eval + futility_margin < alpha) {
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

            if (! board.make_move(move))
                continue;

            /*
            | Add the current move to search stack. |
            */
            ss->move      = move;
            ss->cont_hist = board.history.get_cont_hist_entry(move);

            legals++;
            info.nodes++;

            const int history_score = board.history.get_history(move, ss);

            /*
            | Principal Variation Search and Late Move Reduction [PVS + LMR] (~40 ELO) |
            */
            int score = 0;
            const int new_depth = depth - 1;

            int R = lmr[std::min(63, depth)][std::min(63, legals)] + (pv_node ? 0 : 1);
            R -= (is_quiet_move ? history_score / HISTORY_GRAVITY : 0);
            
            if (depth > 1 && legals > 1) {
                R = std::clamp(R, 1, new_depth);
                int lmr_depth = new_depth - R + 1;
                score = -negamax(board, -alpha - 1, -alpha, lmr_depth, info, local_pv, ss + 1);

                if (score > alpha && R > 0) {
                    score = -negamax(board, -alpha - 1, -alpha, new_depth, info, local_pv, ss + 1);
                }
            }

            else if (!pv_node || legals > 1) {
                score = -negamax(board, -alpha - 1, -alpha, new_depth, info, local_pv, ss + 1);
            }

            if (pv_node && (legals == 1 || (score > alpha && (root_node || score < beta)))) {
                score = -negamax(board, -beta, -alpha, new_depth, info, local_pv, ss + 1);
            }

            board.unmake_move(move, true);

            if (info.stopped)
                return 0;

            if (score > best_score) {
                best_move  = move;
                best_score = score;
                if (score > alpha) {
                    if (pv_node)
                        pv.update(move, score, local_pv);
                    if (score >= beta) {
                        if (is_quiet_move) {
                            if (ss->killers[0] != move) {
                                ss->killers[1] = ss->killers[0];
                                ss->killers[0] = best_move;
                            }
                            board.history.update_countermove(board.get_side_to_move(),
                                                             (ss - 1)->move.get_from(),
                                                             (ss - 1)->move.get_to(), move);
                            board.history.update_history(move.get_from(), move.get_to(), depth,
                                                         bad_quiets);
                            board.history.update_chs(move, ss, bad_quiets, depth);
                        }
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
            return board.is_in_check() ? -MATE + ss->ply : 0;
        }

        tt->store_tt(board.get_hash_key(), best_score, best_move, depth, ss->ply, flag, pv);

        return best_score;
    }

    int get_promo_piece(move::Move move) {
        switch (move.get_promotion()) {
            case move::Promotion::QUEEN:
                return 4;
            case move::Promotion::ROOK:
                return 3;
            case move::Promotion::BISHOP:
                return 2;
            case move::Promotion::KNIGHT:
                return 1;
            default:
                assert(false);
                exit(1);
                return -1;
        }
    }

    bool SEE(const Board &board, const move::Move move, int threshold, const int see_values[7]) {

        if (move.is_promotion())
            return true;

        Square from = move.get_from();
        Square to   = move.get_to();

        int target_piece = move.is_en_passant()
                               ? 0
                               : static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));


        int value = see_values[target_piece] - threshold;

        if (value < 0)
            return false;

        int attacker_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(from)));

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

        Color side = board.piece_color(board.piece_on(from));
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

        return side != board.piece_color(board.piece_on(from));
    }

    void search(Board &board, SearchInfo &info, bool print_info) {
        auto start = std::chrono::high_resolution_clock::now();
        PVariation pv;
        move::Move best_move;

        for (int current_depth = 1; current_depth <= info.depth; current_depth++) {
            info.seldepth = 0;
            int score = 0, alpha = -INF, beta = INF, delta = INITIAL_ASP_DELTA;
            SearchStack stack[MAX_DEPTH + 4], *ss = stack + 4;
            for (int i = -4; i < MAX_DEPTH; i++) {
                (ss + i)->move       = move::NO_MOVE;
                (ss + i)->killers[0] = move::NO_MOVE;
                (ss + i)->killers[1] = move::NO_MOVE;
                (ss + i)->eval       = INF;
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
                score = negamax(board, alpha, beta, current_depth, info, pv, ss);

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

            best_move = (pv.line[0]) ? pv.line[0]
                                          : info.best_root_move;

            if (print_info && pv.line[0]) {
                int time_ms = duration.count();
                int nps     = info.nodes * 1000 / (time_ms + 1);
                if (score > -MATE && score < -MATE_FOUND) {
                    std::cout << "info score mate " << -(score + MATE) / 2 << " depth "
                              << current_depth << " seldepth " << info.seldepth << " nodes "
                              << info.nodes << " time " << time_ms << " nps " << nps << " hashfull "
                              << tt->get_hashfull() << " pv ";
                }

                else if (score > MATE_FOUND && score < MATE) {
                    std::cout << "info score mate " << (MATE - score) / 2 + 1 << " depth "
                              << current_depth << " seldepth " << info.seldepth << " nodes "
                              << info.nodes << " time " << time_ms << " nps " << nps << " hashfull "
                              << tt->get_hashfull() << " pv ";
                }

                else {
                    std::cout << "info score cp " << score << " depth " << current_depth
                              << " seldepth " << info.seldepth << " nodes " << info.nodes
                              << " time " << time_ms << " nps " << nps << " hashfull "
                              << tt->get_hashfull() << " pv ";
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
    }
}