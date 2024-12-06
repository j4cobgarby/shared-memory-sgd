#include "Component/ParaController.hpp"
#include "modular_components.hpp"
#include <sstream>

namespace MiniDNN {

std::vector<PatternController::pattern_element> PatternController::parse_pattern(const std::string &pattern) {
    std::stringstream ss(pattern);
    char type;
    pattern_element elem;
    std::vector<pattern_element> ret;

    while (!ss.eof()) {
        ss >> type;
        switch (type) {
        case 's':
            elem.type = STATIC;
            
            ss >> elem.for_steps;
            ss >> elem.param.static_m;
            break;
        case 'r':
            elem.type = RAMP;

            ss >> elem.for_steps;
            ss >> elem.param.ramp_m1;
            ss >> elem.param.ramp_m2;
            break;
        default:
            std::cerr << "[PatternController] Failed to parse pattern element with type = " << type << std::endl;
            throw -1;
        }
        ret.push_back(elem);
    }

    show_pattern();

    return ret;
}

void PatternController::show_pattern() {
    for (const auto pat : pattern) {
        switch (pat.type) {
        case STATIC:
            std::cout << "STATIC: steps = " << pat.for_steps << ", m = " << pat.param.static_m << "\n";
            break;
        case RAMP:
            std::cout << "RAMP: steps = " << pat.for_steps << ", m = " << pat.param.ramp_m1 << ".." << pat.param.ramp_m2 << "\n";
            break;
        }
    }
}

PatternController::PatternController(SystemExecutor &exec, const std::string &pattern_str)
    : ParaController(exec), pattern(parse_pattern(pattern_str)) {
    for (const auto pat : pattern) {
        this->period += pat.for_steps;
    }
    std::cout << "Pattern period: " << this->period << std::endl;
}

unsigned PatternController::get_parallelism() {
    long step_in_pattern = exec.get_dispatcher()->get_steps_done() % this->period;
    long ret;
    double grad;

    for (int p_i = 0; p_i < pattern.size(); p_i++) {
        const auto pat = pattern.at(p_i);

        if (step_in_pattern < pat.for_steps) {
            // Select this pattern element
            switch (pat.type) {
            case STATIC:
                ret = pat.param.static_m;
                if (this->last_p_i != p_i) {
                    this->last_p_i = p_i;
                    // std::cout << "Changing to STATIC " << ret << "\n";
                    exec.submit_para_change(ret, false);
                }
                return ret;
            case RAMP:
                grad = (double)(pat.param.ramp_m2 - pat.param.ramp_m1)
                        / (double)pat.for_steps;

                ret = pat.param.ramp_m1 + (grad * step_in_pattern);

                if (this->last_p_i != p_i) this->last_reported_ramp_m = -1;

                if (this->last_p_i != p_i || // First step in ramp, or...
                        ( // At a report interval, and a new-found m
                            step_in_pattern % this->ramp_report_interval == 0 &&
                            ((pat.param.ramp_m2 < pat.param.ramp_m1 && ret < last_reported_ramp_m) || (pat.param.ramp_m2 > pat.param.ramp_m1 && ret > last_reported_ramp_m))
                        )) {
                    this->last_p_i = p_i;
                    this->last_reported_ramp_m = ret;
                    // std::cout << "RAMP reporting " << ret << "\n";
                    exec.submit_para_change(ret, false);
                }
                return ret;
            }
        }
        step_in_pattern -= pat.for_steps;
    }

    std::cerr << __FUNCTION__ << " bug: Did not match current step to any pattern element\n";
    return 1;
}

void PatternController::update(long step) {
}

}
