/**
 * @file rk3prey_main.cc
 * @brief NPDE homework RK3Prey code
 * @copyright Developed at ETH Zurich
 */

#include <Eigen/Dense>
#include <vector>

namespace RK3Prey {

// Butcher Tableau based Runge-Kutta explicit solver for autonomous ODEs
/* SAM_LISTING_BEGIN_0 */
class RKIntegrator {
 public:
#if SOLUTION
  // Constructor for the RK method.
  RKIntegrator(const Eigen::MatrixXd &A, const Eigen::VectorXd &b)
      : A_(A), b_(b), s_(b.size()) {
    assert(A.cols() == A.rows() && "Matrix must be square.");
    assert(A.cols() == b.size() && "Incompatible matrix/vector size.");
  }
#else
  RKIntegrator(const Eigen::MatrixXd &A, const Eigen::VectorXd &b) {
    //====================
    // Your code goes here
    //====================
  }
#endif

  // Explicit Runge-Kutta numerical integrator
  template <class Function>
  std::vector<Eigen::VectorXd> solve(const Function &f, double T,
                                     const Eigen::VectorXd &y0,
                                     int M) const;

 private:
#if SOLUTION
  // Butcher data
  const Eigen::MatrixXd A_;
  const Eigen::VectorXd b_;
  int s_;  // size of Butcher tableau
#else
  //====================
  // Your code goes here
  //====================
#endif
};
/* SAM_LISTING_END_0 */

/* Solves an autonomous ODE y' = f(y), y(0) = y0, using a
 * RK scheme from the Butcher tableau provided by the
 * constructor. Performs N equidistant steps up to time T */
/* SAM_LISTING_BEGIN_1 */
template <typename Function>
std::vector<Eigen::VectorXd> RKIntegrator::solve(const Function &f, double T,
                                                 const Eigen::VectorXd &y0,
                                                 int M) const {
  int dim = y0.size(); // dimension
  double h = T / M; // step size
  std::vector<Eigen::VectorXd> sol;
  sol.reserve(M + 1);

#if SOLUTION
  // Initial data
  sol.push_back(y0);

  // RK looping tools
  Eigen::VectorXd incr(dim);
  std::vector<Eigen::VectorXd> k;
  k.reserve(s_);

  // Stepping
  Eigen::VectorXd step(dim);
  for (int iter = 0; iter < M; ++iter) {
    // clear looping variables
    incr.setZero();
    step.setZero();
    k.clear();
    // explicit RK
    k.push_back(f(sol.at(iter)));
    step = step + b_(0) * k.at(0);
    for (int i = 1; i < s_; ++i) {
      for (int j = 0; j < i; ++j) {
        incr = incr + A_(i, j) * k.at(j);
      }
      k.push_back(f(sol.at(iter)+ h * incr));
      step = step + b_(i) * k.back();
    }

    // step forward
    sol.push_back(sol.at(iter) + h * step);
  }
#else
  //====================
  // Your code goes here
  //====================
#endif
  return sol;
}
/* SAM_LISTING_END_1 */

}  // namespace RK3Prey
