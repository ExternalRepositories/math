#ifndef STAN_MATH_FWD_FUN_TCROSSPROD_HPP
#define STAN_MATH_FWD_FUN_TCROSSPROD_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/transpose.hpp>
#include <stan/math/fwd/fun/multiply.hpp>

namespace stan {
namespace math {

template <typename EigMat, require_eigen_vt<is_fvar, EigMat>* = nullptr>
inline Eigen::Matrix<value_type_t<EigMat>, EigMat::RowsAtCompileTime,
                     EigMat::RowsAtCompileTime>
tcrossprod(const EigMat& m) {
  if (m.rows() == 0) {
    return {};
  }
  return multiply(m, m.transpose());
}

}  // namespace math
}  // namespace stan
#endif
