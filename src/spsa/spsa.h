#pragma once
#ifdef USE_TUNE

#include <iostream>
#include <variant>
#include <vector>

#include "defs.h"
#include "evaluation/evaluate.h"
#include "search/history/history_formulas.h"
#include "types.h"
#endif

#ifdef USE_TUNE
namespace elixir::tune {
    struct TunerField {
        TunerField(std::string name, int *value, int default_value, int min_value, int max_value, double lr)
            : name(name), value(value), default_value(default_value), min_value(min_value), max_value(max_value), lr(lr) {
              step = (max_value - min_value) / 20.0;
            }
        TunerField(std::string name, double *value, double default_value, double min_value, double max_value, double lr)
            : name(name), value(value), default_value(default_value), min_value(min_value), max_value(max_value), lr(lr) {
              step = (max_value - min_value) / 20.0;
            }
        std::string name;
        std::variant<int *, double *> value;
        std::variant<int, double> default_value;
        std::variant<int, double> min_value;
        std::variant<int, double> max_value;
        double step;
        double lr;
    };
    class SPSA {
      public:
        SPSA() = default;
        void add_field(TunerField field);
        void print_info();
        void print_current();
        void print_spsa_inputs();
        void update_parameter(const std::string &name, const std::string &option_value);

      private:
        std::vector<TunerField> fields;
    };

    void init_tune();

    extern SPSA tuner;
}
#endif