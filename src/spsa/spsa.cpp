#ifdef USE_TUNE
#include <variant>

#include "evaluation/evaluate.h"
#include "search/movepicker.h"
#include "search/search.h"
#include "spsa.h"
#include "time_manager/time_management_terms.h"
#include "time_manager/time_manager.h"
#endif

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

        // movepicker terms
        // tuner.add_field(TunerField("MP_PAWN", &MP_PAWN, 86, 50, 300, 0.002));
        // tuner.add_field(TunerField("MP_KNIGHT", &MP_KNIGHT, 304, 150, 500, 0.002));
        // tuner.add_field(TunerField("MP_BISHOP", &MP_BISHOP, 360, 200, 600, 0.002));
        // tuner.add_field(TunerField("MP_ROOK", &MP_ROOK, 466, 300, 800, 0.002));
        // tuner.add_field(TunerField("MP_QUEEN", &MP_QUEEN, 905, 500, 1300, 0.002));
        // tuner.add_field(TunerField("MP_KING", &MP_KING, 20903, 10000, 30000, 0.002));
        // tuner.add_field(TunerField("MP_SEE", &MP_SEE, 109, 20, 500, 0.002));

        // history terms
        // tuner.add_field(TunerField("HISTORY_GRAVITY", &HISTORY_GRAVITY, 8289, 1024, 16384, 0.002));
        // tuner.add_field(
        //     TunerField("HISTORY_BONUS_MULTIPLIER", &HISTORY_BONUS_MULTIPLIER, 300, 50, 500, 0.002));
        // tuner.add_field(TunerField("HISTORY_MALUS_MULTIPLIER", &HISTORY_MALUS_MULTIPLIER, 600, 300,
        //                            1200, 0.002));
        // tuner.add_field(
        //     TunerField("HISTORY_BONUS_SUBTRAHEND", &HISTORY_BONUS_SUBTRAHEND, 150, 50, 450, 0.002));
        // tuner.add_field(TunerField("HISTORY_MALUS_SUBTRAHEND", &HISTORY_MALUS_SUBTRAHEND, 300, 100,
        //                            600, 0.002));
        // tuner.add_field(
        //     TunerField("HISTORY_BONUS_MAX", &HISTORY_BONUS_MAX, 4000, 1000, 8000, 0.002));
        // tuner.add_field(
        //     TunerField("HISTORY_MALUS_MAX", &HISTORY_MALUS_MAX, 8000, 2000, 16000, 0.002));

        // see terms
        // tuner.add_field(TunerField("SEE_PAWN", &SEE_PAWN, 77, 20, 300, 0.002));
        // tuner.add_field(TunerField("SEE_KNIGHT", &SEE_KNIGHT, 299, 120, 500, 0.002));
        // tuner.add_field(TunerField("SEE_BISHOP", &SEE_BISHOP, 303, 120, 500, 0.002));
        // tuner.add_field(TunerField("SEE_ROOK", &SEE_ROOK, 504, 300, 700, 0.002));
        // tuner.add_field(TunerField("SEE_QUEEN", &SEE_QUEEN, 903, 500, 1300, 0.002));
        // tuner.add_field(TunerField("SEE_QUIET", &SEE_QUIET, 67, 5, 200, 0.002));
        // tuner.add_field(TunerField("SEE_CAPTURE", &SEE_CAPTURE, 32, 5, 200, 0.002));
        // tuner.add_field(TunerField("QS_SEE_THRESHOLD", &QS_SEE_THRESHOLD, 9, 1, 100, 0.002));
        // tuner.add_field(TunerField("SEE_DEPTH", &SEE_DEPTH, 10, 1, 15, 0.002));

        // aspiration window terms
        // tuner.add_field(TunerField("INITIAL_ASP_DELTA", &INITIAL_ASP_DELTA, 12, 1, 50, 0.002));
        // tuner.add_field(TunerField("ASP_MULTIPLIER", &ASP_MULTIPLIER, 1.3111, 1.0, 5.0, 0.002));
        // tuner.add_field(TunerField("MIN_ASP_DEPTH", &MIN_ASP_DEPTH, 4, 1, 10, 0.002));

        // search terms
        // tuner.add_field(TunerField("IIR_DEPTH", &IIR_DEPTH, 4, 1, 8, 0.002));

        // tuner.add_field(TunerField("RAZOR_MARGIN", &RAZOR_MARGIN, 298, 100, 400, 0.002));
        // tuner.add_field(TunerField("RAZOR_DEPTH", &RAZOR_DEPTH, 7, 2, 10, 0.002));

        // tuner.add_field(TunerField("RFP_MARGIN", &RFP_MARGIN, 72, 20, 350, 0.002));
        // tuner.add_field(TunerField("RFP_DEPTH", &RFP_DEPTH, 6, 2, 10, 0.002));

        // tuner.add_field(TunerField("RFP_BASE", &RFP_BASE, 122, 50, 200, 0.002));
        // tuner.add_field(
        //     TunerField("RFP_BASE_MULTIPLIER", &RFP_BASE_MULTIPLIER, 37, 10, 100, 0.002));

        // tuner.add_field(TunerField("NMP_DEPTH", &NMP_DEPTH, 1, 1, 6, 0.002));
        // tuner.add_field(TunerField("NMP_BASE_REDUCTION", &NMP_BASE_REDUCTION, 5, 2, 10, 0.002));
        // tuner.add_field(TunerField("NMP_DIVISOR", &NMP_DIVISOR, 9, 4, 17, 0.002));
        // tuner.add_field(TunerField("NMP_EVAL_BASE", &NMP_EVAL_BASE, 200, 50, 500, 0.002));
        // tuner.add_field(TunerField("NMP_EVAL_MAX", &NMP_EVAL_MAX, 60, 10, 100, 0.002));
        // tuner.add_field(TunerField("NMP_PHASE_BASE", &NMP_PHASE_BASE, 80, 20, 200, 0.002));
        // tuner.add_field(TunerField("NMP_PHASE_MAX", &NMP_PHASE_MAX, 24, 5, 45, 0.002));

        // tuner.add_field(TunerField("SE_BETA_MULTIPLIER", &SE_BETA_MULTIPLIER, 200, 50, 500, 0.002));
        // tuner.add_field(TunerField("DOUBLE_EXT_MARGIN", &DOUBLE_EXT_MARGIN, 300, 50, 500, 0.002));

        // tuner.add_field(TunerField("LMP_BASE", &LMP_BASE, 2, 2, 14, 0.002));
        // tuner.add_field(TunerField("LMP_MULTIPLIER", &LMP_MULTIPLIER, 1, 1, 8, 0.002));

        // tuner.add_field(TunerField("FP_BASE", &FP_BASE, 146, 20, 500, 0.002));
        // tuner.add_field(TunerField("FP_MULTIPLIER", &FP_MULTIPLIER, 118, 20, 400, 0.002));
        // tuner.add_field(TunerField("FP_DEPTH", &FP_DEPTH, 5, 1, 15, 0.002));

        // tuner.add_field(TunerField("LMR_DEPTH", &LMR_DEPTH, 4, 1, 8, 0.002));
        // tuner.add_field(TunerField("LMR_OFFSET", &LMR_OFFSET, 0.5137, 0.0, 4.0, 0.002));
        // tuner.add_field(TunerField("LMR_DIVISOR", &LMR_DIVISOR, 1.711, 0.5, 10.0, 0.002));
        // tuner.add_field(
        //     TunerField("LMR_HISTORY_BASE", &LMR_HISTORY_BASE, 8289, 1024, 16384, 0.002));
        
        // time management terms
        tuner.add_field(TunerField("BASE_SCALE", &BASE_SCALE, 54, 20, 100, 0.002));
        tuner.add_field(TunerField("INC_SCALE", &INC_SCALE, 60, 20, 100, 0.002));
        tuner.add_field(TunerField("MAX_BOUND_SCALE", &MAX_BOUND_SCALE, 76, 20, 100, 0.002));
        tuner.add_field(TunerField("SOFT_BOUND_SCALE", &SOFT_BOUND_SCALE, 76, 20, 100, 0.002));
        tuner.add_field(TunerField("HARD_BOUND_SCALE", &HARD_BOUND_SCALE, 250, 100, 500, 0.002));

        tuner.add_field(TunerField("MOVE_STABILITY_SCALE_1", &MOVE_STABILITY_SCALE_1, 243, 100, 500, 0.002));
        tuner.add_field(TunerField("MOVE_STABILITY_SCALE_2", &MOVE_STABILITY_SCALE_2, 135, 50, 300, 0.002));
        tuner.add_field(TunerField("MOVE_STABILITY_SCALE_3", &MOVE_STABILITY_SCALE_3, 109, 50, 300, 0.002));
        tuner.add_field(TunerField("MOVE_STABILITY_SCALE_4", &MOVE_STABILITY_SCALE_4, 88, 50, 300, 0.002));
        tuner.add_field(TunerField("MOVE_STABILITY_SCALE_5", &MOVE_STABILITY_SCALE_5, 68, 50, 300, 0.002));
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

        else if (name == "HISTORY_GRAVITY") {
            int history_gravity = std::stoi(option_value);
            HISTORY_GRAVITY     = history_gravity;
        }

        else if (name == "INITIAL_ASP_DELTA") {
            int initial_asp_delta     = std::stoi(option_value);
            search::INITIAL_ASP_DELTA = initial_asp_delta;
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

        else if (name == "BASE_SCALE") {
            int base_scale = std::stoi(option_value);
            BASE_SCALE     = base_scale;
        }

        else if (name == "INC_SCALE") {
            int inc_scale = std::stoi(option_value);
            INC_SCALE     = inc_scale;
        }

        else if (name == "MAX_BOUND_SCALE") {
            int max_bound_scale = std::stoi(option_value);
            MAX_BOUND_SCALE     = max_bound_scale;
        }

        else if (name == "SOFT_BOUND_SCALE") {
            int soft_bound_scale = std::stoi(option_value);
            SOFT_BOUND_SCALE     = soft_bound_scale;
        }

        else if (name == "HARD_BOUND_SCALE") {
            int hard_bound_scale = std::stoi(option_value);
            HARD_BOUND_SCALE     = hard_bound_scale;
        }

        else if (name == "MOVE_STABILITY_SCALE_1") {
            int move_stability_scale_1                       = std::stoi(option_value);
            MOVE_STABILITY_SCALE_1                           = move_stability_scale_1;
            elixir::time_management::move_stability_scale[0] = move_stability_scale_1;
        }

        else if (name == "MOVE_STABILITY_SCALE_2") {
            int move_stability_scale_2                       = std::stoi(option_value);
            MOVE_STABILITY_SCALE_2                           = move_stability_scale_2;
            elixir::time_management::move_stability_scale[1] = move_stability_scale_2;
        }

        else if (name == "MOVE_STABILITY_SCALE_3") {
            int move_stability_scale_3                       = std::stoi(option_value);
            MOVE_STABILITY_SCALE_3                           = move_stability_scale_3;
            elixir::time_management::move_stability_scale[2] = move_stability_scale_3;
        }

        else if (name == "MOVE_STABILITY_SCALE_4") {
            int move_stability_scale_4                       = std::stoi(option_value);
            MOVE_STABILITY_SCALE_4                           = move_stability_scale_4;
            elixir::time_management::move_stability_scale[3] = move_stability_scale_4;
        }

        else if (name == "MOVE_STABILITY_SCALE_5") {
            int move_stability_scale_5                       = std::stoi(option_value);
            MOVE_STABILITY_SCALE_5                           = move_stability_scale_5;
            elixir::time_management::move_stability_scale[4] = move_stability_scale_5;
        }

        else if (name == "NMP_EVAL_BASE") {
            int nmp_eval_base     = std::stoi(option_value);
            search::NMP_EVAL_BASE = nmp_eval_base;
        }

        else if (name == "NMP_EVAL_MAX") {
            int nmp_eval_max     = std::stoi(option_value);
            search::NMP_EVAL_MAX = nmp_eval_max;
        }

        else if (name == "NMP_PHASE_BASE") {
            int nmp_phase_base     = std::stoi(option_value);
            search::NMP_PHASE_BASE = nmp_phase_base;
        }

        else if (name == "NMP_PHASE_MAX") {
            int nmp_phase_max     = std::stoi(option_value);
            search::NMP_PHASE_MAX = nmp_phase_max;
        }

        else if (name == "LMP_BASE") {
            int lmp_base     = std::stoi(option_value);
            search::LMP_BASE = lmp_base;
        }

        // se options
        else if (name == "SE_BETA_MULTIPLIER") {
            int se_beta_multiplier     = std::stoi(option_value);
            search::SE_BETA_MULTIPLIER = se_beta_multiplier;
        }

        else if (name == "DOUBLE_EXT_MARGIN") {
            int double_ext_margin     = std::stoi(option_value);
            search::DOUBLE_EXT_MARGIN = double_ext_margin;
        }

        else if (name == "RFP_BASE") {
            int rfp_base     = std::stoi(option_value);
            search::RFP_BASE = rfp_base;
        }

        else if (name == "RFP_BASE_MULTIPLIER") {
            int rfp_base_multiplier     = std::stoi(option_value);
            search::RFP_BASE_MULTIPLIER = rfp_base_multiplier;
        }

        else if (name == "LMR_HISTORY_BASE") {
            int lmr_history_base     = std::stoi(option_value);
            search::LMR_HISTORY_BASE = lmr_history_base;
        }

        else if (name == "HISTORY_BONUS_MULTIPLIER") {
            int history_bonus_multiplier = std::stoi(option_value);
            HISTORY_BONUS_MULTIPLIER     = history_bonus_multiplier;
        }

        else if (name == "HISTORY_MALUS_MULTIPLIER") {
            int history_malus_multiplier = std::stoi(option_value);
            HISTORY_MALUS_MULTIPLIER     = history_malus_multiplier;
        }

        else if (name == "HISTORY_BONUS_SUBTRAHEND") {
            int history_bonus_subtrahend = std::stoi(option_value);
            HISTORY_BONUS_SUBTRAHEND     = history_bonus_subtrahend;
        }

        else if (name == "HISTORY_MALUS_SUBTRAHEND") {
            int history_malus_subtrahend = std::stoi(option_value);
            HISTORY_MALUS_SUBTRAHEND     = history_malus_subtrahend;
        }

        else if (name == "HISTORY_BONUS_MAX") {
            int history_bonus_max = std::stoi(option_value);
            HISTORY_BONUS_MAX     = history_bonus_max;
        }

        else if (name == "HISTORY_MALUS_MAX") {
            int history_malus_max = std::stoi(option_value);
            HISTORY_MALUS_MAX     = history_malus_max;
        }
    }
}
#endif