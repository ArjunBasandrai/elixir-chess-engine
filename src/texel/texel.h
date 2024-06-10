#pragma once

#include <string>
#include <array>
#include <vector>

#include "../defs.h"
#include "../types.h"
#include "../evaluate.h"

namespace elixir::texel {
    using pair_t = std::array<double, 2>;
    using positions_t = std::vector<std::string>;

    enum Result: I8 {
        BLACK_WIN = -1,
        DRAW = 0,
        WHITE_WIN = 1
    };

    struct Coefficient {
        int index = -1;
        int value = 0;
    };

    struct TunerPosition {
        int phase;
        Color side;
        Result outcome;
        std::vector<Coefficient> coefficients;
    };

    struct Trace {
        std::array<pair_t, 6> material_score;
    };


    inline Trace trace;

    // #define TEXEL
    #ifdef TEXEL
    #define TRACE_ADD(term, count, color) trace.term[color] += count
    #define TRACE_INCREMENT(term, color) trace.term[color]++
    #define TRACE_SCALE(s) trace.scale = s
    #define TRACE_EVAL(e) trace.eval = e
    #else
    #define TRACE_ADD(term, count, color)
    #define TRACE_INCREMENT(term, color)
    #define TRACE_SCALE(s)
    #define TRACE_EVAL(e)
    #endif

    class Tune {
    public:
        Tune() = default;
        ~Tune() = default;

        void add_parameter_single(const EvalScore& param) {
            parameters.push_back((pair_t){static_cast<double>(O(param)), static_cast<double>(E(param))});
            num_params++;
        }

        template <std::size_t N>
        void add_parameter_array(const std::array<EvalScore, N>& param) {
            for (const EvalScore& p : param) {
                add_parameter_single(p);
            }
        }

        template <std::size_t M, std::size_t N>
        void add_parameter_matrix(const std::array<std::array<EvalScore, N>, M>& param) {
            for (const std::array<EvalScore, N>& p : param) {
                add_parameter_array(p);
            }
        }

        void print_parameters() {
            for (const pair_t& p : parameters) {
                std::cout << "S(" << p[0] << ", " << p[1] << ")" << std::endl;
            }
        }
    private:
        int num_params = 0;
        std::vector<pair_t> parameters;
        std::vector<TunerPosition> positions;
        std::vector<pair_t> gradients;
    };

    inline Tune texel;
}