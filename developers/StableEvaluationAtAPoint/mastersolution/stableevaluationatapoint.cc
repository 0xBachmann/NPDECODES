/**
 * @file stableevaluationatapoint.cc
 * @brief NPDE homework StableEvaluationAtAPoint
 * @author Amélie Loher & Erick Schulz
 * @date 22/04/2020
 * @copyright Developed at ETH Zurich
 */
#include "stableevaluationatapoint.h"

namespace StableEvaluationAtAPoint {

double MeshSize(const std::shared_ptr<const lf::mesh::Mesh> &mesh_p) {
  double mesh_size = 0.0;
  // Find maximal edge length
  double edge_length;
  for (const lf::mesh::Entity *edge : mesh_p->Entities(1)) {
    // Compute the length of the edge
    double edge_length =  lf::geometry::Volume(*(edge->Geometry()));
    mesh_size =std::max(edge_length,mesh_size);
  }
  return mesh_size;
}

Eigen::Vector2d OuterNormalUnitSquare(Eigen::Vector2d x){
  if(x(0) > x(1) && x(0) < 1.0-x(1)){
    return Eigen::Vector2d(0.0,-1.0);
  }
  if(x(0) > x(1) && x(0) > 1.0-x(1)){
    return Eigen::Vector2d(1.0,0.0);
  }
  if(x(0) < x(1) && x(0) > 1.0-x(1)){
    return Eigen::Vector2d(0.0,1.0);
  }
  return Eigen::Vector2d(-1.0,0.0);
}


double FundamentalSolution::operator()(Eigen::Vector2d y){
  LF_ASSERT_MSG(x_ != y, "G not defined for these coordinates!");
  return -1.0 / (2.0 * M_PI)  * std::log((x_ - y).norm());
}

Eigen::Vector2d FundamentalSolution::grad(Eigen::Vector2d y){
  LF_ASSERT_MSG(x_ != y, "G not defined for these coordinates!");
  return (x_ - y) / (2.0 * M_PI * (x_- y).squaredNorm());
}

double PointEval(std::shared_ptr<const lf::mesh::Mesh> mesh) {
  double error = 0.0;
#if SOLUTION
  const auto u = [](Eigen::Vector2d x) -> double {
    Eigen::Vector2d one(1.0, 0.0);
    return std::log((x + one).norm());
  };
  const auto gradu = [](Eigen::Vector2d x) -> Eigen::Vector2d {
    Eigen::Vector2d one(1.0, 0.0);
    return (x + one) / (x + one).squaredNorm();
  };
  // Define a Functor for the dot product of grad u(x) * n(x)
  const auto gradu_dot_n = [gradu](const Eigen::Vector2d x) -> double {
    // Determine the normal vector n on the unit square
    Eigen::Vector2d n = OuterNormalUnitSquare(x);
    return gradu(x).dot(n);
  };

  // Compute right hand side
  const Eigen::Vector2d x(0.3, 0.4);
  const double rhs = PSL(mesh, gradu_dot_n, x) - PDL(mesh, u, x);
  // Compute the error
  error = std::abs(u(x) - rhs);
#else
  //====================
  // Your code goes here
  //====================
#endif
  return error;
}

double Psi::operator()(Eigen::Vector2d y){
  const double constant = M_PI / (0.5 * std::sqrt(2) - 1.0);
  const double dist = (y - center_).norm();

  if (dist <= 0.25 * std::sqrt(2)) {
    return 0.0;
  } else if (dist >= 0.5) {
    return 1.0;
  } else {
    return std::pow(std::cos(constant *(dist - 0.5)), 2);
  }
}

Eigen::Vector2d Psi::grad(Eigen::Vector2d y){
  double constant = M_PI / (0.5 * std::sqrt(2) - 1.0);
  double dist = (y - center_).norm();

  if (dist <= 0.25 * std::sqrt(2)) {
    return Eigen::Vector2d(0.0,0.0);

  } else if (dist >= 0.5) {
    return Eigen::Vector2d(0.0,0.0);
  } else {
    return-2.0 * std::cos(constant * (dist - 0.5)) *
                 std::sin(constant * (dist - 0.5)) * (constant / dist) *
                 (y - center_);
  }
}

double Psi::lapl(Eigen::Vector2d y){
  double c = M_PI / (0.5 * std::sqrt(2) - 1.0);
  double c2 = c*c;
  double dist = (y - center_).norm();

  if (dist <= 0.25 * std::sqrt(2)) {
    return  0.0;
  } else if (dist >= 0.5) {
    return 0.0;
  } else {
    double sineval = std::sin(c*(dist-0.5));
    double coseval = std::cos(c*(dist-0.5));
    return 2*c2*sineval*sineval  - 
           2*c2*coseval*coseval -
           2*c*sineval*coseval/dist;
  }
}

//  return (2 * std::pow(constant, 2) / (y - center_).squaredNorm()) *
//             (y - center_).dot(y - center_) *
//             (std::pow(std::sin(constant * (dist - 0.5)), 2) -
//              std::pow(std::cos(constant * (dist - 0.5)), 2)) -
//            (2 * constant / dist) * std::cos(constant * (dist - 0.5)) *
//             std::sin(constant * (dist - 0.5)) *
//             (1.0 - ((y - center_).dot(y - center_) / (y - center_).squaredNorm()));

double Jstar(std::shared_ptr<lf::uscalfe::FeSpaceLagrangeO1<double>> fe_space,
             Eigen::VectorXd uFE, const Eigen::Vector2d x) {
  double val = 0.0;
  Psi psi(Eigen::Vector2d(0.5,0.5));
  FundamentalSolution G(x);
#if SOLUTION

	// Mesh covering a unit square domain
  std::shared_ptr<const lf::mesh::Mesh> mesh = fe_space->Mesh();
  // Use midpoint quadrature rule
  const lf::quad::QuadRule qr = lf::quad::make_TriaQR_MidpointRule();
  // Quadrature points
  const Eigen::MatrixXd zeta_ref{qr.Points()};
  // Quadrature weights
  const Eigen::VectorXd w_ref{qr.Weights()};
  // Number of quadrature points
  const lf::base::size_type P = qr.NumPoints();
  // Create mesh function to be evaluated at the quadrature points
  auto uFE_mf = lf::fe::MeshFunctionFE(fe_space, uFE);

/*
	auto test = [](Eigen::Vector2d x) -> double { 
		return x[0] * x[0] - x[1] * x[1];
	};
	lf::mesh::utils::MeshFunctionGlobal mf_test{test};
*/

	// Loop over all cells
  for (const lf::mesh::Entity *entity : mesh->Entities(0)) {
    // Standard way to apply a local quadrature rule
    const lf::geometry::Geometry &geo{*entity->Geometry()};
    // Quadrature points on actual cell
    const Eigen::MatrixXd zeta{geo.Global(zeta_ref)};
    const Eigen::VectorXd gram_dets{geo.IntegrationElement(zeta_ref)};
    // Values of finite element function on all quadrature points
    auto u_vals = uFE_mf(*entity, zeta_ref);
    
/*
		auto u_vals = mf_test(*entity, zeta_ref);
*/

		// Quadrature loop
    for (int l = 0; l < P; l++) {
      const double w = w_ref[l] *gram_dets[l];
      val += w * (-u_vals[l]) *
             ( 2.0*(G.grad(zeta.col(l))).dot(psi.grad(zeta.col(l))) +
              G(zeta.col(l)) * psi.lapl(zeta.col(l)));       
      
    }
  }

/*
	double testval = test(x);
	std::cout << "testval " << testval << std::endl;
	std::cout << "val " << val <<std::endl;
	std::cout << "err " << testval - val << std::endl;
*/
/*
     double val_test = lf::fe::IntegrateMeshFunction(*mesh, lambda, 9);
  */
#else
  //====================
  // Your code goes here
  //====================
#endif
  return val;
}


double StablePointEvaluation(
    std::shared_ptr<lf::uscalfe::FeSpaceLagrangeO1<double>> fe_space,
    Eigen::VectorXd uFE, const Eigen::Vector2d x) {
  double res = 0.0;

#if SOLUTION
  Eigen::Vector2d center(0.5, 0.5);
  if ((x - center).norm() <= 0.25) {
    res = Jstar(fe_space, uFE, x);
  } else {
    std::cerr << "The point does not fulfill the assumptions" << std::endl;
  }

#else
//====================
// Your code goes here
//====================
#endif

  return res;
}

} //namespace StableEvaluationAtAPoint