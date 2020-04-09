#ifndef STAN_MATH_PRIM_FUN_LOG_MIX_HPP
#define STAN_MATH_PRIM_FUN_LOG_MIX_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/exp.hpp>
#include <stan/math/prim/fun/log.hpp>
#include <stan/math/prim/fun/log1m.hpp>
#include <stan/math/prim/fun/log_sum_exp.hpp>
#include <stan/math/prim/fun/size.hpp>
#include <stan/math/prim/fun/value_of.hpp>
#include <vector>
#include <cmath>

namespace stan {
namespace math {

/**
 * Return the log mixture density with specified mixing proportion
 * and log densities.
 *
 * \f[
 * \mbox{log\_mix}(\theta, \lambda_1, \lambda_2)
 * = \log \left( \theta \lambda_1 + (1 - \theta) \lambda_2 \right).
 * \f]
 *
 * @tparam T_theta type of mixing proportion - must be an arithmetic type
 * @tparam T_lambda1 type of first log density - must be an arithmetic type
 * @tparam T_lambda2 type of second log density - must be an arithmetic type
 * @param[in] theta mixing proportion in [0, 1].
 * @param[in] lambda1 first log density.
 * @param[in] lambda2 second log density.
 * @return log mixture of densities in specified proportion
 */
template <typename T_theta, typename T_lambda1, typename T_lambda2,
          require_all_arithmetic_t<T_theta, T_lambda1, T_lambda2>* = nullptr>
inline double log_mix(T_theta theta, T_lambda1 lambda1, T_lambda2 lambda2) {
  using std::log;
  double theta_double = theta;
  double lambda1_double = lambda1;
  double lambda2_double = lambda2;

  check_not_nan("log_mix", "lambda1", lambda1_double);
  check_not_nan("log_mix", "lambda2", lambda2_double);
  check_bounded("log_mix", "theta", theta_double, 0, 1);
  return log_sum_exp(log(theta_double) + lambda1_double,
                     log1m(theta_double) + lambda2_double);
}

/**
 * Return the log mixture density with specified mixing proportions
 * and log densities.
 *
 * \f[
 * \frac{\partial }{\partial p_x}
 * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+d_n}\right)
 * =\frac{e^{d_x}}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}
 * \f]
 *
 * \f[
 * \frac{\partial }{\partial d_x}
 * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+d_n}\right)
 * =\frac{e^{d_x}p_x}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}
 * \f]
 *
 * @tparam T_theta Type of theta. This can be a scalar, std vector or row/column
 * vector.
 * @tparam T_lam Type of lambda. This can be a scalar, std vector or row/column
 * vector.
 * @param theta std/row/col vector of mixing proportions in [0, 1].
 * @param lambda std/row/col vector of log densities.
 * @return log mixture of densities in specified proportion
 */
template <typename T_theta, typename T_lam,
          require_any_vector_t<T_theta, T_lam>* = nullptr>
return_type_t<T_theta, T_lam> log_mix(const T_theta& theta,
                                      const T_lam& lambda) {
  static const char* function = "log_mix";
  using T_partials_return = partials_return_t<T_theta, T_lam>;
  using T_partials_vec =
      typename Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1>;

  const int N = stan::math::size(theta);

  check_bounded(function, "theta", theta, 0, 1);
  check_not_nan(function, "lambda", lambda);
  check_not_nan(function, "theta", theta);
  check_finite(function, "lambda", lambda);
  check_finite(function, "theta", theta);
  check_consistent_sizes(function, "theta", theta, "lambda", lambda);

  const auto& theta_dbl = value_of(as_column_vector_or_scalar(theta));
  const auto& lam_dbl = value_of(as_column_vector_or_scalar(lambda));

  T_partials_return logp = log_sum_exp(log(theta_dbl) + lam_dbl);
  T_partials_vec theta_deriv = (lam_dbl.array() - logp).exp();

  operands_and_partials<T_theta, T_lam> ops_partials(theta, lambda);
  if (!is_constant_all<T_lam>::value) {
    ops_partials.edge2_.partials_ = theta_deriv.cwiseProduct(theta_dbl);
  }
  if (!is_constant_all<T_theta>::value) {
    ops_partials.edge1_.partials_ = std::move(theta_deriv);
  }
  return ops_partials.build(logp);
}

/**
 * Return the log mixture density given specified mixing proportions
 * and array of log density vectors.
 *
 * \f[
 * \frac{\partial }{\partial p_x}\left[
 * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+d_n}\right)+
 * \log\left(\exp^{\log\left(p_1\right)+f_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+f_n}\right)\right]
 * =\frac{e^{d_x}}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}+
 * \frac{e^{f_x}}{e^{f_1}p_1+\cdot\cdot\cdot+e^{f_m}p_m}
 * \f]
 *
 * \f[
 * \frac{\partial }{\partial d_x}\left[
 * \log\left(\exp^{\log\left(p_1\right)+d_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+d_n}\right)
 * +\log\left(\exp^{\log\left(p_1\right)+f_1}+\cdot\cdot\cdot+
 * \exp^{\log\left(p_n\right)+f_n}\right)\right]
 * =\frac{e^{d_x}p_x}{e^{d_1}p_1+\cdot\cdot\cdot+e^{d_m}p_m}
 * \f]
 *
 * @tparam T_theta Type of theta. This can be a scalar, std vector or row/column
 * vector
 * @tparam T_lam Type of vector in std vector lambda. This can be std vector or
 * row/column vector.
 * @param theta std/row/col vector of mixing proportions in [0, 1].
 * @param lambda std vector containing std/row/col vectors of log densities.
 * @return log mixture of densities in specified proportion
 */
template <typename T_theta, typename T_lam, require_vector_t<T_lam>* = nullptr>
return_type_t<T_theta, std::vector<T_lam>> log_mix(
    const T_theta& theta, const std::vector<T_lam>& lambda) {
  static const char* function = "log_mix";
  using T_partials_return = partials_return_t<T_theta, std::vector<T_lam>>;
  using T_partials_vec =
      typename Eigen::Matrix<T_partials_return, Eigen::Dynamic, 1>;
  using T_partials_mat =
      typename Eigen::Matrix<T_partials_return, Eigen::Dynamic, Eigen::Dynamic>;
  using T_lamvec_type = typename std::vector<T_lam>;

  const int N = stan::math::size(lambda);
  const int M = theta.size();

  check_bounded(function, "theta", theta, 0, 1);
  check_not_nan(function, "theta", theta);
  check_finite(function, "theta", theta);
  for (int n = 0; n < N; ++n) {
    check_not_nan(function, "lambda", lambda[n]);
    check_finite(function, "lambda", lambda[n]);
    check_consistent_sizes(function, "theta", theta, "lambda", lambda[n]);
  }

  const auto& theta_dbl = value_of(as_column_vector_or_scalar(theta));

  T_partials_mat lam_dbl(M, N);
  for (int n = 0; n < N; ++n) {
    lam_dbl.col(n) = value_of(as_column_vector_or_scalar(lambda[n]));
  }

  T_partials_mat logp_tmp = log(theta_dbl).replicate(1, N) + lam_dbl;
  T_partials_vec logp(N);
  for (int n = 0; n < N; ++n) {
    logp[n] = log_sum_exp(logp_tmp.col(n));
  }

  operands_and_partials<T_theta, T_lamvec_type> ops_partials(theta, lambda);
  if (!is_constant_all<T_theta, T_lam>::value) {
    T_partials_mat derivs = exp(lam_dbl - logp.transpose().replicate(M, 1));
    if (!is_constant_all<T_theta>::value) {
      ops_partials.edge1_.partials_ = derivs.rowwise().sum();
    }
    if (!is_constant_all<T_lam>::value) {
      for (int n = 0; n < N; ++n) {
        as_column_vector_or_scalar(
            forward_as<promote_scalar_t<partials_return_t<T_lam>, T_lam>>(
                ops_partials.edge2_.partials_vec_[n]))
            = derivs.col(n).cwiseProduct(theta_dbl);
      }
    }
  }
  return ops_partials.build(logp.sum());
}

}  // namespace math
}  // namespace stan
#endif
