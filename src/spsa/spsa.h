#pragma once
#ifdef USE_TUNE

#include <iostream>
#include <variant>
#include <vector>

#include "defs.h"
#include "evaluate.h"
#include "history.h"
#include "types.h"
#endif

namespace elixir {
    extern int DEFAULT_MOVESTOGO;
    extern int DEFAULT_MOVE_OVERHEAD;
}

#ifdef USE_TUNE
namespace elixir::tune {
    struct TunerField {
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