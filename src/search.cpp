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
#include "utils/bits.h"

using namespace elixir::bits;

namespace elixir::search {
    int LMP_BASE = 7;
    int RFP_MARGIN = 100;
    int RAZOR_MARGIN = 264;
    int NMP_BASE_REDUCTION = 5;
    int NMP_DEPTH = 2;
    int RFP_DEPTH = 6;
    int RAZOR_DEPTH = 6;
    int IIR_DEPTH = 4;
    int LMP_MULTIPLIER = 1;
    int LMR_DEPTH = 3;
}

namespace elixir::search {
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

        if (ss->ply >= MAX_DEPTH - 1) return eval;

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

        /*
        | 3-Fold Repetition Detection (~50 ELO) : If the position has been repeated 3 times, |
        | then the position is a draw.                                                       |
        */
        if (board.is_repetition()) return 0;
        
        /*
        | Check Extension (~25 ELO) : If we are in check, extend the search depth and avoid dropping to qsearch. |
        */
        if (in_check) depth++;

        /*
        | Quiescence Search : Perform a quiescence search at leaf nodes to avoid the horizon effect. |
        */
        if (depth <= 0) return qsearch(board, alpha, beta, info, pv, ss);
        
        if (ss->ply >= MAX_DEPTH - 1) return eval::evaluate(board);

        int legals = 0;

        auto local_pv = PVariation();
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
        if (tt_hit && !pv_node && result.depth >= depth &&
            (tt_flag == TT_EXACT || (tt_flag == TT_ALPHA && result.score <= alpha) || (tt_flag == TT_BETA && result.score >= beta))) {
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
        if (depth >= IIR_DEPTH && tt_move == move::NO_MOVE) depth--;

        /*
        | Initialize the evaluation score. If we are in check, we set the evaluation score to INF.   |
        | Otherwise, if we have a TT hit, we use the stored score. If not, we evaluate the position. |
        */
        if (in_check) eval = ss->eval = INF;

        else eval = ss->eval = (tt_hit) ? result.score : eval::evaluate(board);

        if (!pv_node && !in_check) {
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
            if (depth <= RFP_DEPTH && eval - RFP_MARGIN * depth >= beta) {
                return eval;
            }

            /*
            | Null Move Pruning (~60 ELO) : If our position is so good, we give our |
            | opponent an extra move to see if we are still better.                 |
            */
            if (depth >= NMP_DEPTH && (ss-1)->move != move::NO_MOVE && eval >= beta) {
                int R = NMP_BASE_REDUCTION + depth / 6;
                R = std::min(R, depth);
                
                /*
                | Set current move to a null move in the search stack to avoid |
                | multiple null move searching in a row.                       |
                */
                ss->move = move::NO_MOVE;

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

        TTFlag flag = TT_ALPHA;
        move::Move move;
        bool skip_quiets = false;

        /*
        | Initialize a bad quiets array to be used by history maluses. |
        */
        MoveList bad_quiets;

        while ((move = mp.next_move()) != move::NO_MOVE) {

            const bool is_quiet_move = move.is_quiet();

            /*
            | If skip_quiets is enabled by Late Move Pruning, we skip quiet |
            | moves that are likely to be bad.                              |
            */
            if (skip_quiets && is_quiet_move) continue;

            /*
            | Late Move Pruning [LMP] (~30 ELO) : Skip late quiet moves if  |
            | we've already searched the most promising moves because they  |
            | are likely to be bad.                                         |  
            */
            if (!root_node && best_score > -MATE_FOUND) {
                if (is_quiet_move && legals >= LMP_BASE + LMP_MULTIPLIER * depth * depth) {
                    skip_quiets = true;
                }

                const int see_threshold = is_quiet_move ? -80 * depth : -30 * depth * depth;
                if (depth <= 8 && legals > 0 && !SEE(board, move, see_threshold)) continue;
            }
            
            if (!board.make_move(move)) continue;

            /*
            | Add the current move to search stack. |
            */
            ss->move = move;

            legals++;
            info.nodes++;

            /*
            | Principal Variation Search and Late Move Reduction [PVS + LMR] (~40 ELO) |
            */
            int score = 0;
            /*
            | Search with full depth if it's the first move |
            */
            if (legals == 1) {
                score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv, ss + 1);
            } else {
                /*
                | Late Move Reductions [LMR] : Moves that appear later in the move list |
                | are likely to be bad, so we reduce their depth.                       |
                */
                int R = 1;
                if (is_quiet_move && depth >= LMR_DEPTH && legals > 1 + (pv_node ? 1 : 0)) {
                    R = lmr[std::min(63, depth)][std::min(63, legals)] + (pv_node ? 0 : 1);
                }
                /*
                | Principal Variation Search [PVS] : Perform a null window search at reduced depth |
                | to see if the move has potential to improve alpha. If it does, we perform a full |
                | depth search.                                                                    |
                */
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

    bool SEE(const Board& board, const move::Move move, int threshold) {

        if (move.is_promotion()) return true;

        Square from = move.get_from();
        Square to = move.get_to();

        int target_piece = move.is_en_passant() ? 0 : static_cast<int>(board.piece_to_piecetype(board.piece_on(to)));


        int value = see_values[target_piece] - threshold;

        if (value < 0) return false;

        int attacker_piece = static_cast<int>(board.piece_to_piecetype(board.piece_on(from)));

        value -= see_values[attacker_piece];
        if (value >= 0) return true;
        
        Square sq = move.is_en_passant() ? static_cast<Square>(static_cast<int>(to) - 8 * color_offset[static_cast<int>(board.get_side_to_move())]) : to;
        Bitboard occupied = board.occupancy() ^ bit(from) ^ bit(sq);
        Bitboard attackers = board.get_attackers(to, Color::WHITE, occupied) | board.get_attackers(to, Color::BLACK, occupied);

        Bitboard bishops = board.bishops() | board.queens();
        Bitboard rooks = board.rooks() | board.queens();

        Color side = board.piece_color(board.piece_on(from));
        side = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;

        while (true) {
            attackers &= occupied;

            Bitboard my_attackers = attackers & board.color_occupancy(side);
            if (!my_attackers) break;

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
            occupied ^= bit(static_cast<Square>(lsb_index(my_attackers & board.piece_bitboard(static_cast<PieceType>(piece)))));

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