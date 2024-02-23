#include <iostream>

#include "search.h"

#include "board/board.h"
#include "evaluate.h"
#include "move.h"
#include "movegen.h"
#include "utils/static_vector.h"
#include "utils/timer.h"

namespace elixir::search
{
    void score_moves(Board &board, StaticVector<int, 256> &scores, StaticVector<elixir::move::Move, 256> &moves)
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

    void sort_moves(Board &board, StaticVector<elixir::move::Move, 256> &moves)
    {   
        StaticVector<int, 256> scores;
        scores.resize(moves.size());
        score_moves(board, scores, moves);

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

    int negamax(Board &board, int alpha, int beta, int depth, SearchInfo &info, PVariation &pv)
    {
        pv.length = 0;
        if (info.timed)
        {
            if (info.nodes % 2047 == 0)
            {
                if (timer::m_timer.time() - info.start_time > info.time_left)
                {
                    info.stopped = true;
                    return 0;
                }
            }
        }

        if (depth == 0)
        {
            return eval::evaluate(board);
        }

        int legals = 0;

        auto local_pv = PVariation();
        int best_score = -50000;

        StaticVector<elixir::move::Move, 256> moves = movegen::generate_moves(board);
        sort_moves(board, moves);

        for (const auto &move : moves)
        {
            if (!board.make_move(move))
            {
                continue;
            }
            info.nodes++;
            info.ply++;
            legals++;
            int score = -negamax(board, -beta, -alpha, depth - 1, info, local_pv);
            board.unmake_move(move, true);
            if (info.stopped)
            {
                return 0;
            }
            info.ply--;
            if (score > best_score)
            {
                best_score = score;
                if (score > alpha)
                {
                    alpha = score;
                    pv.load_from(move, local_pv);
                    pv.score = score;
                    if (score >= beta)
                    {
                        return score;
                    }
                }
            }
        }

        if (legals == 0)
        {
            if (board.is_in_check())
            {
                return -49000 + info.ply;
            }
            else
            {
                return 0;
            }
        }

        return best_score;
    }
}