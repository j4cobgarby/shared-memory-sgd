#include "modular_components.hpp"
#include "Component/ParaController.hpp"
#include <unsupported/Eigen/Polynomials>

namespace MiniDNN {

ModellingParaController::ModellingParaController(SystemExecutor &exec, const int num_threads,
                                                 const int num_probes, const int window_size,
                                                 const long probe_steps, const long exec_steps) :
    ParaController(exec),
    probe_steps(probe_steps),
    exec_steps(exec_steps),
    num_probes(num_probes),
    total_workers(num_threads),
    window_size(window_size) {
    switch_to_para(this->total_workers / this->num_probes);
    this->probed_rates.resize(num_probes);
    this->probed_m_values.resize(num_probes);
}

void ModellingParaController::clip_window() {
}

void ModellingParaController::switch_to_para(unsigned m) {
    this->curr_parallelism = this->exec.submit_para_change(m, this->is_probing);
    if (!this->is_probing) this->latest_exec_parallelism = m;
    this->t_stage_start = HRClock::now();
}

void ModellingParaController::update(long step) {
    if (!this->is_probing && step - this->phase_start_step >= this->exec_steps) {
        this->is_probing = true;
        this->current_probe_number = 1;
        this->phase_start_step = step;
        this->loss_start_of_stage = this->exec.get_monitor()->get_loss_accur();
        switch_to_para(this->total_workers / this->num_probes);
    }

    if (this->is_probing) {
        this->mtx.lock();
        const bool should_progress = step - this->phase_start_step >= this->probe_steps;
        if (should_progress) this->phase_start_step = step;
        this->mtx.unlock();

        if (should_progress) {
            const double loss = exec.get_monitor()->get_loss_accur();
            const double stage_dur_s = (double)(HRClock::now() - this->t_stage_start)
                .count() * 1e-9;
            const double stage_convrate = (loss - this->loss_start_of_stage) / stage_dur_s;
            this->loss_start_of_stage = loss;
            std::cout << "[model_probe] Completed probe " << current_probe_number << "/" <<
                num_probes << " with m=" << curr_parallelism << ", L=" << loss << ", delta_L=" <<
                stage_convrate << "\n";
            this->probed_rates.at(this->current_probe_number - 1) = stage_convrate;
            this->probed_m_values.at(this->current_probe_number - 1) = curr_parallelism;

            if (this->current_probe_number >= this->num_probes) {
                this->is_probing = false;
                const auto next_m = (int)estimate_min(probed_m_values, probed_rates, 5, 1,
                                                      total_workers);
                std::cout << "[model_probe] Estimated m* = " << next_m <<
                    ". Starting execution phase.\n";
                switch_to_para(next_m);
                return;
            }

            this->current_probe_number++;
            switch_to_para(this->current_probe_number * (this->total_workers / this->num_probes));
        }
    }
}

double ModellingParaController::estimate_min(std::vector<double> &xs, std::vector<double> &ys,
                                             int degree, double min_x, double max_x) {
    /* Fit a polynomial to the probe results, and then find the minimum */
    const size_t npoints = xs.size();
    Eigen::MatrixXd A(npoints, degree);
    Eigen::VectorXd b(npoints);

    for (int i = 0; i < npoints; i++) {
        const double x_i = xs.at(i);
        b(i) = ys.at(i);
        double acc = 1.0;
        for (int j = 0; j < degree; j++) {
            A(i, j) = acc;
            acc *= x_i;
        }
    }

    Eigen::VectorXd coeffs_egn = A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
    const auto coeffs = std::vector(coeffs_egn.data(), coeffs_egn.data() + coeffs_egn.size());

    std::cout << "Using (x,y) = ";
    for (int i = 0; i < npoints; i++) {
        std::cout << "(" << xs.at(i) << ", " << ys.at(i) << "), ";
    }
    std::cout << "\nApproximated polynomial: ";
    for (int i = 0; i < coeffs.size(); i++) {
        std::cout << coeffs.at(i) << "x^" << i;
        if (i != coeffs.size() - 1) std::cout << " + ";
    }
    std::cout << "\n";

    return find_polynomial_min(coeffs, min_x, max_x).first;
}

double ModellingParaController::eval_polynomial(const std::vector<double> &coeffs, double at_x) {
    double ret = 0;
    for (int i = 0; i < coeffs.size(); i++) {
        ret += coeffs.at(i) * pow(at_x, i);
    }
    return ret;
}

/* TODO: This whole set of polynomial stuff can be optimised a bit by doing everything with linear
 * algebra (but it's fast enough for now; it's only called once before each execution phase) */
std::pair<double, double> ModellingParaController::find_polynomial_min(
    const std::vector<double> &coeffs, const double min_x, const double max_x) {
    Eigen::VectorXd gradient_coeffs(coeffs.size() - 1);

    for (int i = 0; i < coeffs.size() - 1; i++) {
        gradient_coeffs(i) = (double)(i+1) * coeffs.at(i+1);
    }

    Eigen::PolynomialSolver<double, Eigen::Dynamic> solver;
    solver.compute(gradient_coeffs);

    double best_x = min_x, best_y = eval_polynomial(coeffs, min_x);

    if (const double ev = eval_polynomial(coeffs, max_x); ev < best_y) {
        best_x = max_x;
        best_y = ev;
    }

    for (auto root : solver.roots()) {
        const double r_x = root.real();
        if (root.imag() == 0.0 && r_x >= min_x && r_x <= max_x) {
            if (const double ev = eval_polynomial(coeffs, r_x); ev < best_y) {
                best_x = r_x;
                best_y = ev;
            }
        }
    }

    return {best_x, best_y};
}


}