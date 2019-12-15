/**
 * @file
 * @brief NPDE homework SimpleLinearFiniteElements
 * @author Amélie Loher
 * @date 11/12/2019
 * @copyright Developed at ETH Zurich
 */
#include <cmath>
#include <functional>
#include <tuple>

#include <Eigen/Dense>
#include <Eigen/SparseLU>

#include "tria_mesh_2D.h"

namespace SimpleLinearFiniteElements {

Eigen::Matrix<double, 2, 3> gradbarycoordinates(const Eigen::Matrix<double, 2, 3>& Vertices);

Eigen::Matrix3d ElementMatrix_Mass_LFE(const Eigen::Matrix<double, 2, 3>& Vertices);

Eigen::Matrix3d ElementMatrix_Lapl_LFE(const Eigen::Matrix<double, 2, 3>& Vertices);

Eigen::Matrix3d ElementMatrix_LaplMass_LFE(const Eigen::Matrix<double, 2, 3>& Vertices);

Eigen::Vector3d localLoadLFE(const Eigen::Matrix<double, 2, 3>& Vertices,
                             const std::function<double(const Eigen::Vector2d&)>& FHandle);

double L2Norm_squared(const TriaMesh2D& mesh, const Eigen::VectorXd& u);

double L2Error(const SimpleLinearFiniteElements::TriaMesh2D& mesh, const Eigen::VectorXd& uFEM,
               Eigen::Vector3d& exact);

double H1SError(const SimpleLinearFiniteElements::TriaMesh2D& mesh, const Eigen::VectorXd& uFEM,
               const Eigen::Vector2d& exact);

Eigen::SparseMatrix<double> GalerkinAssembly(
    const SimpleLinearFiniteElements::TriaMesh2D& Mesh, const std::function<Eigen::Matrix3d(const Eigen::Matrix<double, 2, 3>&)>& getElementMatrix);

Eigen::VectorXd assemLoad_LFE(const SimpleLinearFiniteElements::TriaMesh2D& Mesh,
                              const std::function<Eigen::Vector3d(const Eigen::Matrix<double, 2, 3>&, std::function<double(const Eigen::Vector2d&)>)>& getElementVector,
                              const std::function<double(const Eigen::Vector2d&)>& FHandle);

std::tuple<Eigen::VectorXd, double, double> solve(const SimpleLinearFiniteElements::TriaMesh2D &mesh);

} // namespace SimpleLinearFiniteElements 

