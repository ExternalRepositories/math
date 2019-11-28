#include <test/unit/math/test_ad.hpp>

TEST(MathMixMatFun, qrThinR) {
  auto f = [](const auto& x) { return stan::math::qr_thin_R(x); };

  Eigen::MatrixXd a(0, 0);
  stan::test::expect_ad(f, a);

  Eigen::MatrixXd b(1, 1);
  b << 1.5;
  stan::test::expect_ad(f, b);

  stan::test::ad_tolerances tols;
  tols.hessian_hessian_ = 1e-2;
  tols.hessian_fvar_hessian_ = 1e-2;

  Eigen::MatrixXd c(3, 2);
  c << 1, 2, 3, 4, 5, 6;
  stan::test::expect_ad(tols, f, c);

  // exception because cols > rows
  Eigen::MatrixXd b_tr = b.transpose();
  stan::test::expect_ad(f, b_tr);
}
