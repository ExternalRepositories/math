#ifndef STAN_MATH_PRIM_MAT_FUN_OPENCL_TRIDIAGONALIZATION_HPP
#define STAN_MATH_PRIM_MAT_FUN_OPENCL_TRIDIAGONALIZATION_HPP

#ifdef STAN_OPENCL

#include <Eigen/QR>
#include <iostream>

#include <stan/math/opencl/matrix_cl.hpp>
#include <stan/math/opencl/multiply.hpp>
#include <stan/math/opencl/subtract.hpp>
#include <stan/math/opencl/add.hpp>
#include <stan/math/opencl/transpose.hpp>

#include <stan/math/opencl/kernels/tridiagonalization.hpp>

using namespace std;

//#define TIME_IT
//#define SKIP_Q

namespace stan {
namespace math {

void s(const Eigen::MatrixXd& a) {
  std::cout << "(" << a.rows() << ", " << a.cols() << ")" << std::endl;
}

void s(const Eigen::VectorXd& a) {
  std::cout << "(" << a.rows() << ", " << a.cols() << ")" << std::endl;
}

void s(const Eigen::RowVectorXd& a) {
  std::cout << "(" << a.rows() << ", " << a.cols() << ")" << std::endl;
}

void p(const Eigen::MatrixXd& a) {
  s(a);
  std::cout << a << std::endl;
}


void p(const Eigen::VectorXd& a) {
  s(a);
  std::cout << a << std::endl;
}

void p(const Eigen::RowVectorXd& a) {
  s(a);
  std::cout << a << std::endl;
}

void p(const matrix_cl& a) {
  Eigen::MatrixXd b(a.rows(), a.cols());
  copy(b, a);
  s(b);
  std::cout << b << std::endl;
}

/**
 * Tridiagonalize a symmetric matrix using block Housholder algorithm. A = Q * T * Q^T, where T is tridiagonal and Q is orthonormal.
 * @param A Input matrix
 * @param[out] packed Packed form of the tridiagonal matrix. Elements of the resulting symmetric tridiagonal matrix T are in the diagonal and first superdiagonal.
 * Columns bellow diagonal contain householder vectors that can be used to construct orthogonal matrix Q.
 * @param r Block size. Affects only performance of the algorithm. Optimal value depends on the size of A and cache of the processor. For larger matrices or larger cache sizes a larger value is optimal.
 */
void block_householder_tridiag(const Eigen::MatrixXd& A, Eigen::MatrixXd& packed, int r = 60) {
  packed = A;
#ifdef TIME_IT
  int t1=0, t2=0, t3=0, t4=0;
  auto start = std::chrono::steady_clock::now();
#endif
  for (size_t k = 0; k < packed.rows() - 2; k += r) {
    int actual_r = std::min({r, static_cast<int>(packed.rows() - k - 2)});
    Eigen::MatrixXd V(packed.rows() - k - 1, actual_r);

    for (size_t j = 0; j < actual_r; j++) {
#ifdef TIME_IT
      start = std::chrono::steady_clock::now();
#endif
      auto householder = packed.col(k + j).tail(packed.rows() - k - j - 1);
      if (j != 0) {
        auto householder_whole = packed.col(k + j).tail(packed.rows() - k - j);
        householder_whole -= packed.block(j + k, k, householder_whole.size(), j) * V.block(j - 1, 0, 1, j).transpose() +
                             V.block(j - 1, 0, householder_whole.size(), j) * packed.block(j + k, k, 1, j).transpose();
      }
      double q = householder.squaredNorm();
      double alpha = -copysign(sqrt(q), packed(k + j, k + j));
      q -= householder[0] * householder[0];
      householder[0] -= alpha;
      q += householder[0] * householder[0];
      q = sqrt(q);
      householder *= SQRT_2 / q;
#ifdef TIME_IT
      t1+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
      start = std::chrono::steady_clock::now();
#endif

      auto& u = householder;
      Eigen::VectorXd v(householder.size() + 1);
      v.tail(householder.size()) = packed.bottomRightCorner(packed.rows() - k - j - 1, packed.cols() - k - j - 1).selfadjointView<Eigen::Lower>() * u
                                   - packed.block(k + j + 1, k, u.size(), j) * (V.bottomLeftCorner(u.size(), j).transpose() * u)
                                   - V.bottomLeftCorner(u.size(), j) * (packed.block(k + j + 1, k, u.size(), j).transpose() * u);
      v[0] = q / SQRT_2;
      double cnst = v.tail(householder.size()).transpose() * u;
      v.tail(householder.size()) -= 0.5 * cnst * u;
#ifdef TIME_IT
      t2 += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
      start = std::chrono::steady_clock::now();
#endif

      packed(k + j, k + j + 1) = packed(k + j + 1, k + j) * q / SQRT_2 + alpha - v[0] * u[0];
      V.col(j).tail(V.rows() - j) = v.tail(V.rows() - j);
#ifdef TIME_IT
      t3+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif
    }
#ifdef TIME_IT
    start = std::chrono::steady_clock::now();
#endif
    Eigen::MatrixXd partial_update = packed.block(k + actual_r, k,packed.rows() - k - actual_r, actual_r) * V.bottomRows(V.rows() - actual_r + 1).transpose();
    packed.block(k + actual_r, k + actual_r, packed.rows() - k - actual_r, packed.cols() - k - actual_r).triangularView<Eigen::Lower>() -= partial_update + partial_update.transpose();
#ifdef TIME_IT
    t4+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif
  }
  packed(packed.rows() - 2, packed.cols() - 1) = packed(packed.rows() - 1, packed.cols() - 2);
#ifdef TIME_IT
  std::cout << "block_householder_tridiag5 detailed timings: " << t1/1000 << " " << t2/1000 << " " << t3/1000 << " " << t4/1000 << std::endl;
#endif
}

/**
 * Tridiagonalize a symmetric matrix using block Housholder algorithm. A = Q * T * Q^T, where T is tridiagonal and Q is orthonormal.
 * @param A Input matrix
 * @param[out] packed Packed form of the tridiagonal matrix. Elements of the resulting symmetric tridiagonal matrix T are in the diagonal and first superdiagonal.
 * Columns bellow diagonal contain householder vectors that can be used to construct orthogonal matrix Q.
 * @param r Block size. Affects only performance of the algorithm. Optimal value depends on the size of A and cache of the processor. For larger matrices or larger cache sizes a larger value is optimal.
 */
void block_householder_tridiag_cl(const Eigen::MatrixXd& A, Eigen::MatrixXd& packed, int r = 60) {
  matrix_cl packed_gpu(A);
#ifdef TIME_IT
  int t1=0, t2=0, t3=0, t4=0, t5=0, t6=0, t7=0, t8=0, t9=0;
  auto start = std::chrono::steady_clock::now();
#endif
  for (size_t k = 0; k < A.rows() - 2; k += r) {
    int actual_r = std::min({r, static_cast<int>(A.rows() - k - 2)});

#ifdef TIME_IT
    start = std::chrono::steady_clock::now();
#endif
    matrix_cl V_gpu(A.rows() - k - 1, actual_r+1);
#ifdef TIME_IT
    t1+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif

    for (size_t j = 0; j < actual_r; j++) {
#ifdef TIME_IT
      start = std::chrono::steady_clock::now();
#endif
      matrix_cl Uu(j,1), Vu(j,1), q_gpu(1,1);
      try{
        opencl_kernels::tridiagonalization_householder(
                cl::NDRange(1024), cl::NDRange(1024),
                packed_gpu.buffer(), V_gpu.buffer(), q_gpu.buffer(),
                packed_gpu.rows(), V_gpu.rows(), j, k);
#ifdef TIME_IT
        t2+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
#endif
        if(j!=0) {
          opencl_kernels::tridiagonalization_v1(
                  cl::NDRange(64 * j), cl::NDRange(64),
                  packed_gpu.buffer(), V_gpu.buffer(), Uu.buffer(), Vu.buffer(),
                  packed_gpu.rows(), V_gpu.rows(), k);
        }
#ifdef TIME_IT
        t3+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
#endif
        opencl_kernels::tridiagonalization_v2(
                cl::NDRange((A.rows() - k - j - 1 + 63)/64*64),cl::NDRange(64),
                packed_gpu.buffer(), V_gpu.buffer(), Uu.buffer(), Vu.buffer(),
                packed_gpu.rows(), V_gpu.rows(), k, j);
#ifdef TIME_IT
        t4+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
        start = std::chrono::steady_clock::now();
#endif
        opencl_kernels::tridiagonalization_v3(
                cl::NDRange(128),cl::NDRange(128),
                packed_gpu.buffer(), V_gpu.buffer(), q_gpu.buffer(),
                packed_gpu.rows(), V_gpu.rows(), k, j);
      }
      catch (cl::Error& e) {
        check_opencl_error("block_householder_tridiag_cl", e);
      }
#ifdef TIME_IT
      t5+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif
    }
#ifdef TIME_IT
    start = std::chrono::steady_clock::now();
#endif
    matrix_cl U_gpu(V_gpu.rows() - actual_r + 1, actual_r);
    U_gpu.sub_block(packed_gpu, k + actual_r, k, 0, 0, A.rows() - k - actual_r, actual_r);
    matrix_cl Vb_gpu(V_gpu.rows() - actual_r + 1, actual_r);
    Vb_gpu.sub_block(V_gpu, actual_r - 1, 0, 0, 0, V_gpu.rows() - actual_r + 1, actual_r);
#ifdef TIME_IT
    t6+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    start = std::chrono::steady_clock::now();
#endif
    matrix_cl partial_update_gpu = U_gpu * transpose(Vb_gpu);
#ifdef TIME_IT
    t7+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    start = std::chrono::steady_clock::now();
#endif
    try{
      opencl_kernels::subtract_twice(
              cl::NDRange(partial_update_gpu.rows(), partial_update_gpu.cols()),
              packed_gpu.buffer(), partial_update_gpu.buffer(),
              packed_gpu.rows(), partial_update_gpu.rows(), k + actual_r);
    }
    catch (cl::Error& e) {
      check_opencl_error("block_householder_tridiag_cl", e);
    }
#ifdef TIME_IT
    t8+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    start = std::chrono::steady_clock::now();
#endif
  }
#ifdef TIME_IT
  start = std::chrono::steady_clock::now();
#endif
  packed.resize(A.rows(),A.cols());
  copy(packed, packed_gpu);
  packed(packed.rows() - 2, packed.cols() - 1) = packed(packed.rows() - 1, packed.cols() - 2);
#ifdef TIME_IT
  t9+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
  start = std::chrono::steady_clock::now();
#endif
#ifdef TIME_IT
  std::cout << "block_householder_tridiag_cl2 detailed timings: " << t1/1000 << " " << t2/1000 << " " << t3/1000 << " " << t4/1000 << " " << t5/1000  << " " << t6/1000  << " " << t7/1000  << " " << t8/1000  << " " << t9/1000 << std::endl;
#endif
}


/**
 * Calculates Q*A in place. To construct Q pass identity matrix as input A.
 * @param packed Packed result of tridiagonalization that contains householder vectors that define Q in columns bellow the diagonal. Usually result of a call to `block_householder_tridiag3`.
 * @param[in,out] A On input a matrix to multiply with Q. On output the product Q*A.
 * @param r Block size. Affects only performance of the algorithm. Optimal value depends on the size of A and cache of the processor. For larger matrices or larger cache sizes a larger value is optimal.
 */
void block_apply_packed_Q(const Eigen::MatrixXd& packed, Eigen::MatrixXd& A, int r = 100) {
  //if input A==Identity, constructs Q
  Eigen::MatrixXd scratchSpace(A.rows(), r);
  for (int k = (packed.rows() - 3) / r * r; k >= 0; k -= r) {
    int actual_r = std::min({r, static_cast<int>(packed.rows() - k - 2)});
    Eigen::MatrixXd W(packed.rows() - k - 1, actual_r);
    W.col(0) = packed.col(k).tail(W.rows());
    for (size_t j = 1; j < actual_r; j++) {
      scratchSpace.col(0).head(j).noalias() = packed.block(k + j + 1, k, packed.rows() - k - j - 1, j).transpose() * packed.col(j + k).tail(packed.rows() - k - j - 1);
      W.col(j).noalias() = -W.leftCols(j) * scratchSpace.col(0).head(j);
      W.col(j).tail(W.rows() - j) += packed.col(j + k).tail(packed.rows() - k - j - 1);
    }
    scratchSpace.transpose().bottomRows(actual_r).noalias() = packed.block(k + 1, k, packed.rows() - k - 1, actual_r).transpose().triangularView<Eigen::Upper>() * A.bottomRows(A.rows() - k - 1);
    A.bottomRows(A.cols() - k - 1).noalias() -= W * scratchSpace.transpose().bottomRows(actual_r);
  }
}

/**
 * Calculates Q*A in place. To construct Q pass an appropriate identity matrix as input A.
 * @param packed Packed result of tridiagonalization that contains householder vectors that define Q in columns bellow the diagonal. Usually result of a call to `block_householder_tridiag3`.
 * @param[in,out] A On input a matrix to multiply with Q. On output the product Q*A.
 * @param r Block size. Affects only performance of the algorithm. Optimal value depends on the size of A and cache of the processor. For larger matrices or larger cache sizes larger value is optimal.
 */
void block_apply_packed_Q_cl(const Eigen::MatrixXd& packed, Eigen::MatrixXd& A, int r = 200) {
  auto start = std::chrono::steady_clock::now();
  matrix_cl A_gpu(A);
  Eigen::MatrixXd scratchSpace(A.rows(), r);
#ifdef TIME_IT
  int t0=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
  int t1=0, t2=0, t3=0, t4=0;
#endif
  for (int k = (packed.rows() - 3) / r * r; k >= 0; k -= r) {
    int actual_r = std::min({r, static_cast<int>(packed.rows() - k - 2)});
    Eigen::MatrixXd W(packed.rows() - k - 1, actual_r);
    W.col(0) = packed.col(k).tail(W.rows());
    for (size_t j = 1; j < actual_r; j++) {
#ifdef TIME_IT
      start = std::chrono::steady_clock::now();
#endif
      scratchSpace.col(0).head(j).noalias() = packed.block(k + j + 1, k, packed.rows() - k - j - 1, j).transpose() * packed.col(j + k).tail(packed.rows() - k - j - 1);
#ifdef TIME_IT
      t1+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
      start = std::chrono::steady_clock::now();
#endif
      W.col(j).noalias() = -W.leftCols(j) * scratchSpace.col(0).head(j);
      W.col(j).tail(W.rows() - j) += packed.col(j + k).tail(packed.rows() - k - j - 1);
#ifdef TIME_IT
      t2+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif
    }
#ifdef TIME_IT
    start = std::chrono::steady_clock::now();
#endif
    Eigen::MatrixXd packed_block_transpose_triang = packed.block(k + 1, k, packed.rows() - k - 1, actual_r).transpose().triangularView<Eigen::Upper>();
    matrix_cl packed_block_transpose_triang_gpu(packed_block_transpose_triang);
    matrix_cl A_bottom_gpu(A.rows() - k - 1, A.cols());
    A_bottom_gpu.sub_block(A_gpu, k+1, 0, 0, 0, A_bottom_gpu.rows(), A_bottom_gpu.cols());
    matrix_cl W_gpu(W);
#ifdef TIME_IT
    t3+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
    start = std::chrono::steady_clock::now();
#endif
    A_bottom_gpu = A_bottom_gpu - W_gpu * tri_rect_multiply<TriangularViewCL::Upper>(packed_block_transpose_triang_gpu, A_bottom_gpu);
    A_gpu.sub_block(A_bottom_gpu, 0, 0, k+1, 0, A_bottom_gpu.rows(), A_bottom_gpu.cols());
#ifdef TIME_IT
    t4+=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
#endif
  }
  copy(A,A_gpu);
#ifdef TIME_IT
  std::cout << "block_apply_packed_Q_cl2 detailed timings: " << t0/1000 << " " << t1/1000 << " " << t2/1000 << " " << t3/1000 << " " << t4/1000 << std::endl;
#endif
}

}
}

#endif
#endif
