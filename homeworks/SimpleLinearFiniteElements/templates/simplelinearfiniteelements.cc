/**
 * @file
 * @brief NPDE homework SimpleLinearFiniteElements
 * @author Amélie Loher
 * @date 11/12/2019
 * @copyright Developed at ETH Zurich
 */

#include "simplelinearfiniteelements.h"

namespace SimpleLinearFiniteElements {

/* SAM_LISTING_BEGIN_9 */
double getArea(const Eigen::Matrix<double, 2, 3> &triangle) {
  return std::abs(
      0.5 *
      ((triangle(0, 1) - triangle(0, 0)) * (triangle(1, 2) - triangle(1, 1)) -
       (triangle(0, 2) - triangle(0, 1)) * (triangle(1, 1) - triangle(1, 0))));
}
/* SAM_LISTING_END_9 */

Eigen::Matrix<double, 2, 3> gradbarycoordinates(
    const Eigen::Matrix<double, 2, 3> &triangle) {
  Eigen::Matrix3d X;

  // solve for the coefficients of the barycentric coordinate functions, see
  // \eqref{eq:lambdalse}
  X.block<3, 1>(0, 0) = Eigen::Vector3d::Ones();
  X.block<3, 2>(0, 1) = triangle.transpose();
  return X.inverse().block<2, 3>(1, 0);
}

/**
 *  @brief Computation of Element Matrix for the Laplacian
 */
Eigen::Matrix3d ElementMatrix_Lapl_LFE(
    const Eigen::Matrix<double, 2, 3> &triangle) {
  Eigen::Matrix<double, 2, 3> X = gradbarycoordinates(triangle);
  // compute inner products of gradients through matrix multiplication
  return getArea(triangle) * X.transpose() * X;
}

/**
 *  @brief Computation of full Galerkin Matrix
 */
Eigen::Matrix3d ElementMatrix_LaplMass_LFE(
    const Eigen::Matrix<double, 2, 3> &triangle) {
  return ElementMatrix_Lapl_LFE(triangle) + ElementMatrix_Mass_LFE(triangle);
}

/**
 *  @brief Computation of element mass matrix on planar triangle
 *  @param triangle 2x3 matrix of vertex coordinates
 */
/* SAM_LISTING_BEGIN_1 */
Eigen::Matrix3d ElementMatrix_Mass_LFE(
    const Eigen::Matrix<double, 2, 3> &triangle) {
  Eigen::Matrix3d element_matrix;
  //====================
  // Your code goes here
  //====================
  return element_matrix;
}
/* SAM_LISTING_END_1 */


/**
 * @brief H1Serror Computes the H^1 error between the approximate solution and
 *                the exact solution
 * @param mesh the mesh to use
 * @param uFEM the solution approximated through FEM
 * @param exact the exact gradient of the solution
 * @return the H^1 difference
 *
 * @note This implementation seems to be flawed!
 */
/* SAM_LISTING_BEGIN_3 */
double H1Serror(
    const TriaMesh2D &mesh, const Eigen::VectorXd &uFEM,
    const std::function<Eigen::Vector2d(const Eigen::Vector2d &)> exact) {
  double H1Serror_squared = 0.0;
  //====================
  // Your code goes here
  //====================

  return std::sqrt(H1Serror_squared);
}
/* SAM_LISTING_END_3 */

/**
 * @brief assemLoad_LFE Assembles the Load Vector
 * @param mesh the mesh to use
 * @param getElementVector
 * @param f function handle for f
 * @return assembled load vector
 */
Eigen::VectorXd assemLoad_LFE(
    const TriaMesh2D &mesh,
    const std::function<double(const Eigen::Vector2d &)> &f) {
  // obtain the number of triangles
  int M = mesh.elements.rows();

  // obtain the number of vertices
  int N = mesh.vertices.rows();
  Eigen::VectorXd phi = Eigen::VectorXd::Zero(N);

  // loop over all triangles
  for (int i = 0; i < M; i++) {
    Eigen::Matrix<double, 2, 3> triangle = mesh[i];

    // loop over vertices of current triangle
    double factor = getArea(triangle) / 3.0;
    for (int j = 0; j < 3; ++j) {
      // from local to global load vector
      phi(mesh.elements(i, j)) += factor * f(triangle.col(j));
    }
  }

  return phi;
}

/**
 * @brief GalerkinAssembly Assembles the Galerkin Matrix
 * @param mesh the mesh to use
 * @param getElementMatrix Element Matrix
 * @return Galerkin Matrix
 */
Eigen::SparseMatrix<double> GalerkinAssembly(
    const TriaMesh2D &mesh,
    const std::function<Eigen::Matrix3d(const Eigen::Matrix<double, 2, 3> &)>
        &getElementMatrix) {
  // obtain the number of vertices
  int N = mesh.vertices.rows();
  // obtain the number of elements/cells
  int M = mesh.elements.rows();
  std::vector<Eigen::Triplet<double>> triplets;
  // loop over elements and add local contributions
  for (int i = 0; i < M; i++) {
    // get local$\to$global index mapping for current element, \emph{cf.}
    // \lref{eq:idxdef}
    Eigen::Vector3i element = mesh.elements.row(i);
    Eigen::Matrix<double, 2, 3> triangle;
    // extract vertices of current element
    for (int j = 0; j < 3; j++) {
      triangle.col(j) = mesh.vertices.row(element(j)).transpose();
    }
    // compute element contributions
    Eigen::Matrix3d Ak = getElementMatrix(triangle);
    // build triplets from contributions
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        triplets.push_back({element(j), element(k), Ak(j, k)});
      }
    }
  }
  // build sparse matrix from triplets
  Eigen::SparseMatrix<double> A(N, N);
  A.setFromTriplets(triplets.begin(), triplets.end());
  A.makeCompressed();
  return A;
}

/**
 * @brief solves system and prints H1-semierror, L2 error, the mesh and a
 * surface plot
 * @param mesh: discretisation of the computational domain
 */
/* SAM_LISTING_BEGIN_4 */
std::tuple<Eigen::VectorXd, double, double> Solve(
    const SimpleLinearFiniteElements::TriaMesh2D &mesh) {
  const double pi = 3.1415926535897;

  // define the source function f
  auto f = [pi](const Eigen::Vector2d &x) {
    return (1.0 + 8.0 * pi * pi) * std::cos(2.0 * pi * x(0)) *
           std::cos(2.0 * pi * x(1));
  };
  // the exact solution of the linear variational problem
  auto uExact = [pi](const Eigen::Vector2d &x) {
    return std::cos(2 * pi * x(0)) * std::cos(2 * pi * x(1));
  };

  Eigen::VectorXd U;
  double l2error;
  double h1error;

  //====================
  // Your code goes here
  // Assigning some dummy values
  U = Eigen::VectorXd::Zero(mesh.vertices.rows());
  l2error = 1.0;
  h1error = 1.0;
  //====================
  return std::make_tuple(U, l2error, h1error);
}
/* SAM_LISTING_END_4 */

}  // namespace SimpleLinearFiniteElements
