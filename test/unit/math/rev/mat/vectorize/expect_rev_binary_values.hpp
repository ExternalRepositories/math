#ifndef TEST_UNIT_MATH_REV_MAT_VECTORIZE_EXPECT_REV_BINARY_VALUES_HPP
#define TEST_UNIT_MATH_REV_MAT_VECTORIZE_EXPECT_REV_BINARY_VALUES_HPP

#include <test/unit/math/rev/mat/vectorize/expect_rev_binary_scalar_value.hpp>
#include <test/unit/math/rev/mat/vectorize/expect_rev_binary_std_vector_value.hpp>
#include <test/unit/math/rev/mat/vectorize/expect_rev_binary_matrix_value.hpp>
#include <Eigen/Dense>

// Also will test derivatives
template <typename F>
void expect_rev_binary_values() {
  expect_rev_binary_scalar_value<F>();
  expect_rev_binary_std_vector_value<F>();
  Eigen::MatrixXd model_matrix;
  Eigen::VectorXd model_vector;
  Eigen::RowVectorXd model_row_vector;
  expect_rev_binary_matrix_value<F>(model_matrix);
  expect_rev_binary_matrix_value<F>(model_vector);
  expect_rev_binary_matrix_value<F>(model_row_vector);
}

#endif
