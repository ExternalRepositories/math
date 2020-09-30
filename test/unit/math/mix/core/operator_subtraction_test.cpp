#include <test/unit/math/test_ad.hpp>

TEST(mathMixCore, operatorSubtraction) {
  auto f = [](const auto& x1, const auto& x2) { return x1 - x2; };
  bool disable_lhs_int = true;
  stan::test::expect_common_binary(f, disable_lhs_int);
  stan::test::expect_complex_common_binary(f);
}

TEST(mathMixCore, operatorSubtractionMatrixSmall) {
  // This calls operator+ under the hood
  auto f = [](const auto& x1, const auto& x2) { return stan::math::subtract(x1, x2); };
  stan::test::ad_tolerances tols;
  tols.hessian_hessian_ = 1e-1;
  tols.hessian_fvar_hessian_ = 1e-1;

  double scalar_a = 10;
  Eigen::VectorXd vector_v1(1);
  vector_v1 << 3;
  Eigen::RowVectorXd row_vector_rv1(1);
  row_vector_rv1 << -2;
  Eigen::MatrixXd matrix_m11(1, 1);
  matrix_m11 << 1.5;
  stan::test::expect_ad(tols, f, scalar_a, vector_v1);
  stan::test::expect_ad(tols, f, vector_v1, scalar_a);
  stan::test::expect_ad(tols, f, scalar_a, row_vector_rv1);
  stan::test::expect_ad(tols, f, row_vector_rv1, scalar_a);
  stan::test::expect_ad(tols, f, scalar_a, matrix_m11);
  stan::test::expect_ad(tols, f, matrix_m11, scalar_a);
  stan::test::expect_ad(tols, f, matrix_m11, matrix_m11);

  stan::test::expect_ad_matvar(tols, f, scalar_a, vector_v1);
  stan::test::expect_ad_matvar(tols, f, vector_v1, scalar_a);
  stan::test::expect_ad_matvar(tols, f, scalar_a, row_vector_rv1);
  stan::test::expect_ad_matvar(tols, f, row_vector_rv1, scalar_a);
  stan::test::expect_ad_matvar(tols, f, row_vector_rv1, vector_v1);
  stan::test::expect_ad_matvar(tols, f, vector_v1, row_vector_rv1);
  stan::test::expect_ad_matvar(tols, f, scalar_a, matrix_m11);
  stan::test::expect_ad_matvar(tols, f, matrix_m11, scalar_a);
  stan::test::expect_ad_matvar(tols, f, matrix_m11, vector_v1);
  stan::test::expect_ad_matvar(tols, f, row_vector_rv1, matrix_m11);
  stan::test::expect_ad_matvar(tols, f, matrix_m11, matrix_m11);

}

TEST(mathMixCore, operatorSubtractionMatrixZeroSize) {
  auto f = [](const auto& x1, const auto& x2) { return stan::math::subtract(x1, x2); };
  stan::test::ad_tolerances tols;
  tols.hessian_hessian_ = 1e-1;
  tols.hessian_fvar_hessian_ = 1e-1;
  double scalar_a = 10;
  Eigen::VectorXd vector_v0(0);
  Eigen::RowVectorXd rowvector_rv0(0);
  Eigen::MatrixXd matrix_m00(0, 0);
  stan::test::expect_ad(f, scalar_a, vector_v0);
  stan::test::expect_ad(f, vector_v0, scalar_a);
  stan::test::expect_ad(f, scalar_a, rowvector_rv0);
  stan::test::expect_ad(f, rowvector_rv0, scalar_a);
  stan::test::expect_ad(f, scalar_a, matrix_m00);
  stan::test::expect_ad(f, matrix_m00, scalar_a);
  stan::test::expect_ad(f, matrix_m00, matrix_m00);

  stan::test::expect_ad_matvar(f, scalar_a, vector_v0);
  stan::test::expect_ad_matvar(f, vector_v0, scalar_a);
  stan::test::expect_ad_matvar(f, scalar_a, rowvector_rv0);
  stan::test::expect_ad_matvar(f, rowvector_rv0, scalar_a);
  stan::test::expect_ad_matvar(f, scalar_a, matrix_m00);
  stan::test::expect_ad_matvar(f, matrix_m00, scalar_a);
  stan::test::expect_ad_matvar(f, matrix_m00, vector_v0);
  stan::test::expect_ad_matvar(f, rowvector_rv0, vector_v0);
  stan::test::expect_ad_matvar(f, vector_v0, rowvector_rv0);
  stan::test::expect_ad_matvar(f, rowvector_rv0, matrix_m00);
  stan::test::expect_ad_matvar(f, matrix_m00, matrix_m00);
}

TEST(mathMixCore, operatorSubtractionMatrixNormal) {
  auto f = [](const auto& x1, const auto& x2) { return stan::math::subtract(x1, x2); };
  stan::test::ad_tolerances tols;
  tols.hessian_hessian_ = 1e-1;
  tols.hessian_fvar_hessian_ = 1e-1;
  double scalar_a = 10;
  Eigen::VectorXd vector_v(2);
  vector_v << 100, -3;
  Eigen::RowVectorXd rowvector_rv(2);
  rowvector_rv << 100, -3;
  Eigen::MatrixXd matrix_m(2, 2);
  matrix_m << 100, 0, -3, 4;
  stan::test::expect_ad(tols, f, scalar_a, vector_v);
  stan::test::expect_ad(tols, f, vector_v, scalar_a);
  stan::test::expect_ad(tols, f, scalar_a, rowvector_rv);
  stan::test::expect_ad(tols, f, rowvector_rv, scalar_a);
  stan::test::expect_ad(tols, f, scalar_a, matrix_m);
  stan::test::expect_ad(tols, f, matrix_m, matrix_m);
  stan::test::expect_ad(tols, f, matrix_m, scalar_a);

  stan::test::expect_ad_matvar(tols, f, scalar_a, vector_v);
  stan::test::expect_ad_matvar(tols, f, vector_v, scalar_a);
  stan::test::expect_ad_matvar(tols, f, scalar_a, rowvector_rv);
  stan::test::expect_ad_matvar(tols, f, rowvector_rv, scalar_a);
  stan::test::expect_ad_matvar(tols, f, rowvector_rv, vector_v);
  stan::test::expect_ad_matvar(tols, f, vector_v, rowvector_rv);
  stan::test::expect_ad_matvar(tols, f, scalar_a, matrix_m);
  stan::test::expect_ad_matvar(tols, f, matrix_m, scalar_a);
  stan::test::expect_ad_matvar(tols, f, matrix_m, vector_v);
  stan::test::expect_ad_matvar(tols, f, rowvector_rv, matrix_m);
  stan::test::expect_ad_matvar(tols, f, matrix_m, matrix_m);
}

TEST(mathMixCore, operatorSubtractionMatrixFailures) {
  auto f = [](const auto& x1, const auto& x2) { return stan::math::subtract(x1, x2); };
  stan::test::ad_tolerances tols;
  tols.hessian_hessian_ = 1e-1;
  tols.hessian_fvar_hessian_ = 1e-1;
  // These two tests below are just to make sure both fail correctly.
  Eigen::RowVectorXd d1(3);
  d1 << 1, 3, -5;
  Eigen::VectorXd d2(3);
  d2 << 4, -2, -1;
  stan::test::expect_ad_matvar(tols, f, d1, d2);
  stan::test::expect_ad_matvar(tols, f, d2, d1);

  Eigen::MatrixXd u(3, 2);
  u << 1, 3, -5, 4, -2, -1;
  Eigen::MatrixXd u_tr = u.transpose();
  Eigen::VectorXd vv(2);
  vv << -2, 4;
  Eigen::RowVectorXd rvv(3);
  rvv << -2, 4, 1;
  stan::test::expect_ad_matvar(tols, f, u, u_tr);
  stan::test::expect_ad_matvar(tols, f, u_tr, u);
  stan::test::expect_ad_matvar(tols, f, u, vv);
  stan::test::expect_ad_matvar(tols, f, rvv, u);

}
