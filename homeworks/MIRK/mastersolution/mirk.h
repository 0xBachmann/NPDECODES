#ifndef MIRK_H_
#define MIRK_H_

/**
 * @file mirk.h
 * @brief NPDE homework MIRK code
 * @author Unknown, Oliver Rietmann
 * @date 04.04.2021
 * @copyright Developed at ETH Zurich
 */

#include <Eigen/LU>

namespace MIRK {

//! \brief Perform 2 steps of newton method applied to F and its jacobian DF
//! \tparam Function type for function F
//! \tparam Jacobian type for Jacobian DF of F
//! \param[in] F function F, for which F(z) = 0 is needed
//! \param[in] DF Jacobian DF of the function F
//! \param[in] z0 initial guess
//! \return final approximation for z such that F(z) = 0
/* SAM_LISTING_BEGIN_0 */
template <class Function, class Jacobian>
Eigen::VectorXd newton2steps(Function &&F, Jacobian &&DF, Eigen::VectorXd z) {
  // First Newton step
  Eigen::VectorXd znew = z - DF(z).lu().solve(F(z));
  // Second Newton step
  z = znew - DF(znew).lu().solve(F(znew));
  return z;
}
/* SAM_LISTING_END_0 */

//! \brief Perform a single step of the MIRK scheme applied to the scalar ODE y'
//! = f(y) \tparam Function type for function f \tparam Jacobian type for
//! jacobian df of f \param[in] f function f, as in y' = f(y) \param[in] df
//! Jacobian df of the function f \param[in] y0 previous value \param[in] h
//! step-size \return value y1 at next step
/* SAM_LISTING_BEGIN_1 */
template <class Function, class Jacobian>
double mirkStep(Function &&f, Jacobian &&df, double y0, double h) {
  // Coefficients of MIRK
  const double v1 = 1.0;
  const double v2 = 344.0 / 2025.0;
  const double d21 = -164.0 / 2025.0;
  const double b1 = 37.0 / 82.0;
  const double b2 = 45.0 / 82.0;

  // F derived from MIRK scheme (vector valued)
  auto F = [&f, &y0, &v1, &v2, &d21, &b1, &b2,
            &h](Eigen::Vector3d z) -> Eigen::Vector3d {
    Eigen::Vector3d ret;
    ret << z(0) - (1 - v1) * y0 - v1 * z(2),
        z(1) - (1 - v2) * y0 - v2 * z(2) - h * d21 * f(z(0)),
        z(2) - y0 - h * (b1 * f(z(0)) + b2 * f(z(1)));
    return ret;
  };
  // Jacobian of F (matrix-valued)
  auto DF = [&df, &v1, &v2, &d21, &b1, &b2,
             &h](Eigen::Vector3d z) -> Eigen::Matrix3d {
    Eigen::Matrix3d M;
    M << 0, 0, v1, h * d21 * df(z(0)), 0, v2, h * b1 * df(z(0)),
        h * b2 * df(z(1)), 0;
    return Eigen::Matrix3d::Identity() - M;
  };
  // Initial Guess for Newton steps
  Eigen::Vector3d z(0.0, 0.0, y0);
  // Approximate z, s.t. F(z) = 0

  return newton2steps(F, DF, z)(2);
}
/* SAM_LISTING_END_1 */

//! \brief Solve an ODE y' = f(y) using MIRK scheme on equidistant steps
//! \tparam Function type for function f
//! \tparam Jacobian type for jacobian df of f
//! \param[in] f function f, as in y' = f(y)
//! \param[in] df Jacobian df of the function f
//! \param[in] y0 initial value
//! \param[in] T final time
//! \param[in] N number of steps
//! \return value approximating y(T)
/* SAM_LISTING_BEGIN_2 */
template <class Function, class Jacobian>
double mirkSolve(Function &&f, Jacobian &&df, double y0, double T,
                 unsigned int N) {
  // Step size
  const double h = T / N;
  // Will contain next step
  double ynext = y0;
  for (unsigned int i = 0; i < N; ++i) {
    // Do one step
    ynext = mirkStep(std::forward<Function>(f), std::forward<Jacobian>(df),
                     ynext, h);
  }
  // Return final value at t = T
  return ynext;
}
/* SAM_LISTING_END_2 */

}  // namespace MIRK

#endif  // #ifndef MIRK_H_
