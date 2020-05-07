#ifndef STAN_MATH_FWD_FUN_UNIT_VECTOR_CONSTRAIN_HPP
#define STAN_MATH_FWD_FUN_UNIT_VECTOR_CONSTRAIN_HPP

#include <stan/math/fwd/core.hpp>
#include <stan/math/fwd/fun/tcrossprod.hpp>
#include <stan/math/fwd/fun/sqrt.hpp>
#include <stan/math/prim/fun/divide.hpp>
#include <stan/math/prim/fun/dot_self.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/inv.hpp>
#include <stan/math/prim/fun/unit_vector_constrain.hpp>
#include <stan/math/prim/fun/tcrossprod.hpp>
#include <cmath>

namespace stan {
namespace math {

template <typename EigMat, require_eigen_vt<is_fvar, EigMat>* = nullptr>
inline auto unit_vector_constrain(EigMat&& y) {
  using eig_mat = std::decay_t<EigMat>;
  using eigen_type = typename eig_mat::PlainObject;
  using eig_partial = partials_type_t<value_type_t<EigMat>>;
  using partial_mat = Eigen::Matrix<eig_partial, eig_mat::RowsAtCompileTime,
                                    eig_mat::ColsAtCompileTime>;
  partial_mat y_val(value_of(y));
  eigen_type unit_vector_y(y.size());
  unit_vector_y.val() = unit_vector_constrain(y_val);

  auto squared_norm = dot_self(y_val);
  auto norm = sqrt(squared_norm);
  auto inv_norm = inv(norm);
  Eigen::Matrix<eig_partial, Eigen::Dynamic, Eigen::Dynamic> J
      = divide(tcrossprod(y_val), -norm * squared_norm);

  for (Eigen::Index m = 0; m < y_val.size(); ++m) {
    J.coeffRef(m, m) += inv_norm;
    for (Eigen::Index k = 0; k < y_val.size(); ++k) {
      unit_vector_y.coeffRef(k).d_ = J.coeff(k, m);
    }
  }
  return unit_vector_y;
}

template <typename EigMat, typename T,
          require_eigen_vt<is_fvar, EigMat>* = nullptr,
          require_stan_scalar_t<T>* = nullptr>
inline auto unit_vector_constrain(EigMat&& y, T& lp) {
  const value_type_t<EigMat> squared_norm = dot_self(y);
  lp -= 0.5 * squared_norm;
  return unit_vector_constrain(std::forward<EigMat>(y));
}

}  // namespace math
}  // namespace stan
#endif
