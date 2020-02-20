#include <stan/math/prim.hpp>
#include <gtest/gtest.h>

TEST(ProbBinomial, ccdf_log_matches_lccdf) {
  int n = 3;
  int N = 6;
  double theta = 0.4;

  EXPECT_FLOAT_EQ((stan::math::binomial_lccdf(n, N, theta)),
                  (stan::math::binomial_ccdf_log(n, N, theta)));
  EXPECT_FLOAT_EQ((stan::math::binomial_lccdf<int>(n, N, theta)),
                  (stan::math::binomial_ccdf_log<int>(n, N, theta)));
}
