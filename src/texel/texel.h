#pragma once

#include <array>
#include <string>
#include <vector>

#include "../defs.h"
#include "../evaluate.h"
#include "../types.h"

using namespace elixir::eval;

namespace elixir::texel {
    using pair_t      = std::array<double, 2>;
    using positions_t = std::vector<std::string>;

    enum Result : I8 { BLACK_WIN = -1, DRAW = 0, WHITE_WIN = 1 };

    struct Coefficient {
        int index = -1;
        int value = 0;
    };

    struct TunerPosition {
        int coeff_index = -1;
        int phase;
        Result outcome;
        int eval = 0;
        std::vector<Coefficient> coefficients;
    };

    struct TunerHyperParams {
        double learning_rate = 1.0;
        double K             = 0.0;
    };

    struct Trace {
        std::array<std::array<int, 2>, 6> material_score;
    };

    inline Trace trace;

#define TEXEL
#ifdef TEXEL
#define TRACE_ADD(term, count, color) trace.term[color] += count
#define TRACE_INCREMENT(term, color) trace.term[color]++
#define TRACE_DECREMENT(term, color) trace.term[color]--
#else
#define TRACE_ADD(term, count, color)
#define TRACE_INCREMENT(term, color)
#define TRACE_DECREMENT(term, color)
#endif

    class Tune {
      public:
        Tune()  = default;
        ~Tune() = default;

        void get_coefficient_value_single(TunerPosition &position,
                                          const std::array<int, 2> &param) {
            Coefficient coeff;
            coeff.index = ++position.coeff_index;
            coeff.value = param[0] - param[1];
            position.coefficients.push_back(coeff);
        }

        template <std::size_t N>
        void get_coefficient_value_array(TunerPosition &position,
                                         const std::array<std::array<int, 2>, N> &param) {
            for (const auto &p : param) {
                get_coefficient_value_single(position, {p[0], p[1]});
            }
        }

        void get_all_coefficients(TunerPosition &position) {
            get_coefficient_value_array<6>(position, trace.material_score);
        }

        void add_parameter_single(const EvalScore &param) {
            parameters.push_back(
                (pair_t){static_cast<double>(O(param)), static_cast<double>(E(param))});
            num_params++;
        }

        template <std::size_t N> void add_parameter_array(const std::array<EvalScore, N> &param) {
            for (const EvalScore &p : param) {
                add_parameter_single(p);
            }
        }

        template <std::size_t M, std::size_t N>
        void add_parameter_matrix(const std::array<std::array<EvalScore, N>, M> &param) {
            for (const std::array<EvalScore, N> &p : param) {
                add_parameter_array(p);
            }
        }

        void print_parameters() {
            for (const pair_t &p : parameters) {
                std::cout << "S(" << p[0] << ", " << p[1] << ")" << std::endl;
            }
        }

        void get_intial_parameters();
        void load_data(std::vector<std::string> files);
        void get_eval();
        [[nodiscard]] inline double sigmoid(Score score, const double K) const {
            return 1.0 / (1.0 + std::exp(-K * score / 400.0));
        }
        double get_error(const double K) const;
        void set_optimal_k();

      private:
        int num_params = 0;
        std::vector<pair_t> parameters;
        std::vector<TunerPosition> positions;
        std::vector<pair_t> gradients;
        positions_t fens;
        TunerHyperParams hyper_parameters;

        void read_epd(std::string filename);
        void create_entry(Board &board, std::string line);
    };

    inline Tune texel;
}