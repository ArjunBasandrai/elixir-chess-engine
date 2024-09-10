#ifdef USE_TUNE
#include <variant>

#include "evaluate.h"
#include "movepicker.h"
#include "search.h"
#include "spsa.h"
#endif

namespace elixir {
    int DEFAULT_MOVESTOGO     = -1;
    int DEFAULT_MOVE_OVERHEAD = 100;
}

#ifdef USE_TUNE
namespace elixir::tune {

    SPSA tuner;

    void SPSA::add_field(TunerField field) {
        fields.push_back(field);
    }

    void SPSA::print_info() {
        for (auto &field : fields) {
            const bool is_int = std::holds_alternative<int>(field.default_value);
            std::cout << "option name " << field.name;
            std::cout << " type " << (is_int ? "spin" : "string");
            std::cout << " default "
                      << (is_int ? std::get<int>(field.default_value)
                                 : std::get<double>(field.default_value));
            std::cout << " min "
                      << (is_int ? std::get<int>(field.min_value)
                                 : std::get<double>(field.min_value));
            std::cout << " max "
                      << (is_int ? std::get<int>(field.max_value)
                                 : std::get<double>(field.max_value));
            std::cout << std::endl;
        }
    }

    void SPSA::print_current() {
        for (auto &field : fields) {
            const bool is_int = std::holds_alternative<int>(field.default_value);
            std::cout << field.name << ": ";
            std::cout << (is_int ? *std::get<int *>(field.value)
                                 : *std::get<double *>(field.value));
            std::cout << std::endl;
        }
    }

    void init_tune() {
        using namespace elixir::eval;
        using namespace elixir::search;
        // eval terms
        tuner.add_field({"TEMPO", &TEMPO, 7, 1, 100, 1, 0.002});

        // movepicker terms
        tuner.add_field({"MP_PAWN", &MP_PAWN, 86, 50, 300, 10, 0.002});
        tuner.add_field({"MP_KNIGHT", &MP_KNIGHT, 304, 150, 500, 10, 0.002});
        tuner.add_field({"MP_BISHOP", &MP_BISHOP, 360, 200, 600, 10, 0.002});
        tuner.add_field({"MP_ROOK", &MP_ROOK, 466, 300, 800, 25, 0.002});
        tuner.add_field({"MP_QUEEN", &MP_QUEEN, 905, 500, 1300, 40, 0.002});
        tuner.add_field({"MP_KING", &MP_KING, 20903, 10000, 30000, 500, 0.002});
        tuner.add_field({"MP_SEE", &MP_SEE, 109, 20, 500, 5, 0.002});

        // history terms
        tuner.add_field({"HISTORY_GRAVITY", &HISTORY_GRAVITY, 8289, 1024, 16384, 32, 0.002});

        // time management terms
        tuner.add_field({"DEFAULT_MOVESTOGO", &DEFAULT_MOVESTOGO, 19, 5, 50, 1, 0.002});
        tuner.add_field({"DEFAULT_MOVE_OVERHEAD", &DEFAULT_MOVE_OVERHEAD, 48, 5, 150, 5, 0.002});

        // see terms
        tuner.add_field({"SEE_PAWN", &SEE_PAWN, 77, 20, 300, 5, 0.002});
        tuner.add_field({"SEE_KNIGHT", &SEE_KNIGHT, 299, 120, 500, 10, 0.002});
        tuner.add_field({"SEE_BISHOP", &SEE_BISHOP, 303, 120, 500, 10, 0.002});
        tuner.add_field({"SEE_ROOK", &SEE_ROOK, 504, 300, 700, 10, 0.002});
        tuner.add_field({"SEE_QUEEN", &SEE_QUEEN, 903, 500, 1300, 10, 0.002});
        tuner.add_field({"SEE_QUIET", &SEE_QUIET, 67, 5, 200, 3, 0.002});
        tuner.add_field({"SEE_CAPTURE", &SEE_CAPTURE, 32, 5, 200, 2, 0.002});
        tuner.add_field({"SEE_DEPTH", &SEE_DEPTH, 10, 1, 15, 1, 0.002});
        tuner.add_field({"QS_SEE_THRESHOLD", &QS_SEE_THRESHOLD, 9, 1, 100, 1, 0.002});

        // aspiration window terms
        tuner.add_field({"INITIAL_ASP_DELTA", &INITIAL_ASP_DELTA, 12, 1, 50, 1, 0.002});
        tuner.add_field({"ASP_MULTIPLIER", &ASP_MULTIPLIER, 1.3111, 1.0, 5.0, 0.25, 0.002});
        tuner.add_field({"MIN_ASP_DEPTH", &MIN_ASP_DEPTH, 4, 1, 10, 1, 0.002});

        // search terms
        tuner.add_field({"LMP_BASE", &LMP_BASE, 2, 2, 14, 1, 0.002});
        tuner.add_field({"RFP_MARGIN", &RFP_MARGIN, 72, 20, 350, 20, 0.002});
        tuner.add_field({"RAZOR_MARGIN", &RAZOR_MARGIN, 298, 100, 400, 15, 0.002});
        tuner.add_field({"NMP_BASE_REDUCTION", &NMP_BASE_REDUCTION, 5, 2, 10, 0.75, 0.002});
        tuner.add_field({"NMP_DEPTH", &NMP_DEPTH, 1, 1, 6, 0.85, 0.002});
        tuner.add_field({"RFP_DEPTH", &RFP_DEPTH, 6, 2, 10, 1, 0.002});
        tuner.add_field({"RAZOR_DEPTH", &RAZOR_DEPTH, 7, 2, 10, 1, 0.002});
        tuner.add_field({"IIR_DEPTH", &IIR_DEPTH, 4, 1, 8, 0.75, 0.002});
        tuner.add_field({"LMP_MULTIPLIER", &LMP_MULTIPLIER, 1, 1, 8, 0.75, 0.002});
        tuner.add_field({"LMR_DEPTH", &LMR_DEPTH, 4, 1, 8, 1, 0.002});
        tuner.add_field({"FP_BASE", &FP_BASE, 146, 20, 500, 10, 0.002});
        tuner.add_field({"FP_MULTIPLIER", &FP_MULTIPLIER, 118, 20, 400, 5, 0.002});
        tuner.add_field({"FP_DEPTH", &FP_DEPTH, 5, 1, 15, 1, 0.002});
        tuner.add_field({"NMP_DIVISOR", &NMP_DIVISOR, 9, 4, 17, 1, 0.002});
        tuner.add_field({"LMR_OFFSET", &LMR_OFFSET, 0.5137, 0.0, 4.0, 0.05, 0.002});
        tuner.add_field({"LMR_DIVISOR", &LMR_DIVISOR, 1.711, 0.5, 10.0, 0.1, 0.002});
    }

    void SPSA::print_spsa_inputs() {
        for (auto &field : fields) {
            const bool is_int = std::holds_alternative<int>(field.default_value);
            std::cout << field.name << ", " << (is_int ? "int" : "float") << ", ";
            std::cout << (is_int ? std::get<int>(field.default_value)
                                 : std::get<double>(field.default_value))
                      << ", ";
            std::cout << (is_int ? std::get<int>(field.min_value)
                                 : std::get<double>(field.min_value))
                      << ", ";
            std::cout << (is_int ? std::get<int>(field.max_value)
                                 : std::get<double>(field.max_value))
                      << ", ";
            std::cout << field.step << ", " << field.lr;
            std::cout << std::endl;
        }
    }

    void SPSA::update_parameter(const std::string &name, const std::string &option_value) {
        if (name == "LMP_BASE") {
            int lmp_base     = std::stoi(option_value);
            search::LMP_BASE = lmp_base;
        }

        else if (name == "RFP_MARGIN") {
            int rfp_margin     = std::stoi(option_value);
            search::RFP_MARGIN = rfp_margin;
        }

        else if (name == "RAZOR_MARGIN") {
            int razor_margin     = std::stoi(option_value);
            search::RAZOR_MARGIN = razor_margin;
        }

        else if (name == "NMP_BASE_REDUCTION") {
            int nmp_base_reduction     = std::stoi(option_value);
            search::NMP_BASE_REDUCTION = nmp_base_reduction;
        }

        else if (name == "NMP_DEPTH") {
            int nmp_depth     = std::stoi(option_value);
            search::NMP_DEPTH = nmp_depth;
        }

        else if (name == "NMP_DIVISOR") {
            int nmp_divisor     = std::stoi(option_value);
            search::NMP_DIVISOR = nmp_divisor;
        }

        else if (name == "RFP_DEPTH") {
            int rfp_depth     = std::stoi(option_value);
            search::RFP_DEPTH = rfp_depth;
        }

        else if (name == "RAZOR_DEPTH") {
            int razor_depth     = std::stoi(option_value);
            search::RAZOR_DEPTH = razor_depth;
        }

        else if (name == "IIR_DEPTH") {
            int iir_depth     = std::stoi(option_value);
            search::IIR_DEPTH = iir_depth;
        }

        else if (name == "LMP_MULTIPLIER") {
            int lmp_multiplier     = std::stoi(option_value);
            search::LMP_MULTIPLIER = lmp_multiplier;
        }

        else if (name == "LMR_DEPTH") {
            int lmr_depth     = std::stoi(option_value);
            search::LMR_DEPTH = lmr_depth;
        }

        else if (name == "LMR_OFFSET") {
            float lmr_offset   = std::stof(option_value);
            search::LMR_OFFSET = lmr_offset;
            search::init_lmr();
        }

        else if (name == "LMR_DIVISOR") {
            float lmr_divisor   = std::stof(option_value);
            search::LMR_DIVISOR = lmr_divisor;
            search::init_lmr();
        }

        else if (name == "FP_BASE") {
            int fp_base     = std::stoi(option_value);
            search::FP_BASE = fp_base;
        }

        else if (name == "FP_MULTIPLIER") {
            int fp_multiplier     = std::stoi(option_value);
            search::FP_MULTIPLIER = fp_multiplier;
        }

        else if (name == "FP_DEPTH") {
            int fp_depth     = std::stoi(option_value);
            search::FP_DEPTH = fp_depth;
        }

        else if (name == "SEE_QUIET") {
            int see_quiet     = std::stoi(option_value);
            search::SEE_QUIET = see_quiet;
        }

        else if (name == "SEE_CAPTURE") {
            int see_capture     = std::stoi(option_value);
            search::SEE_CAPTURE = see_capture;
        }

        else if (name == "SEE_DEPTH") {
            int see_depth     = std::stoi(option_value);
            search::SEE_DEPTH = see_depth;
        }

        else if (name == "QS_SEE_THRESHOLD") {
            int qs_see_threshold     = std::stoi(option_value);
            search::QS_SEE_THRESHOLD = qs_see_threshold;
        }

        else if (name == "SEE_PAWN") {
            int see_pawn     = std::stoi(option_value);
            search::SEE_PAWN = see_pawn;
        }

        else if (name == "SEE_KNIGHT") {
            int see_knight     = std::stoi(option_value);
            search::SEE_KNIGHT = see_knight;
        }

        else if (name == "SEE_BISHOP") {
            int see_bishop     = std::stoi(option_value);
            search::SEE_BISHOP = see_bishop;
        }

        else if (name == "SEE_ROOK") {
            int see_rook     = std::stoi(option_value);
            search::SEE_ROOK = see_rook;
        }

        else if (name == "SEE_QUEEN") {
            int see_queen     = std::stoi(option_value);
            search::SEE_QUEEN = see_queen;
        }

        else if (name == "MP_SEE") {
            int mp_see = std::stoi(option_value);
            MP_SEE     = mp_see;
        }

        else if (name == "MIN_ASP_DEPTH") {
            int min_asp_depth     = std::stoi(option_value);
            search::MIN_ASP_DEPTH = min_asp_depth;
        }

        else if (name == "DEFAULT_MOVESTOGO") {
            int default_movestogo = std::stoi(option_value);
            DEFAULT_MOVESTOGO     = default_movestogo;
        }

        else if (name == "DEFAULT_MOVE_OVERHEAD") {
            int default_move_overhead = std::stoi(option_value);
            DEFAULT_MOVE_OVERHEAD     = default_move_overhead;
        }

        else if (name == "HISTORY_GRAVITY") {
            int history_gravity = std::stoi(option_value);
            HISTORY_GRAVITY     = history_gravity;
        }

        else if (name == "INITIAL_ASP_DELTA") {
            int initial_asp_delta     = std::stoi(option_value);
            search::INITIAL_ASP_DELTA = initial_asp_delta;
        }

        else if (name == "TEMPO") {
            int tempo   = std::stoi(option_value);
            eval::TEMPO = tempo;
        }

        else if (name == "ASP_MULTIPLIER") {
            float asp_multiplier   = std::stof(option_value);
            search::ASP_MULTIPLIER = asp_multiplier;
        }

        else if (name == "MP_PAWN") {
            int mp_pawn   = std::stoi(option_value);
            eval::MP_PAWN = mp_pawn;
        }

        else if (name == "MP_KNIGHT") {
            int mp_knight   = std::stoi(option_value);
            eval::MP_KNIGHT = mp_knight;
        }

        else if (name == "MP_BISHOP") {
            int mp_bishop   = std::stoi(option_value);
            eval::MP_BISHOP = mp_bishop;
        }

        else if (name == "MP_ROOK") {
            int mp_rook   = std::stoi(option_value);
            eval::MP_ROOK = mp_rook;
        }

        else if (name == "MP_QUEEN") {
            int mp_queen   = std::stoi(option_value);
            eval::MP_QUEEN = mp_queen;
        }

        else if (name == "MP_KING") {
            int mp_king   = std::stoi(option_value);
            eval::MP_KING = mp_king;
        }
    }
}
#endif