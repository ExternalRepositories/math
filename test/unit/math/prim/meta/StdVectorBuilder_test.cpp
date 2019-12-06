#include <stan/math/prim/meta.hpp>
#include <gtest/gtest.h>
#include <vector>

TEST(MetaTraitsPrimScal, StdVectorBuilder_false_false) {
  using stan::StdVectorBuilder;
  using stan::length;

  double a_double(1);

  StdVectorBuilder<false, double, double> dvv1(length(a_double));
  EXPECT_THROW(dvv1[0], std::logic_error);
  EXPECT_THROW(dvv1.data(), std::logic_error);
}

TEST(MetaTraitsPrimScal, StdVectorBuilder_true_false) {
  using stan::StdVectorBuilder;
  using stan::length;

  double a_double(1);

  StdVectorBuilder<true, double, double> dvv1(length(a_double));
  EXPECT_FLOAT_EQ(0.0, dvv1[0]);
  EXPECT_FLOAT_EQ(0.0, dvv1[1]);
  EXPECT_FLOAT_EQ(0.0, dvv1[100]);
  double data1 = 0;
  EXPECT_NO_THROW(data1 = dvv1.data());
  EXPECT_FLOAT_EQ(0.0, data1);
}

TEST(MetaTraitsPrimScal, StdVectorBuilder_type_check) {
  using stan::StdVectorBuilder;
  using stan::contains_std_vector;

  bool r
      = contains_std_vector<StdVectorBuilder<true, double, int>::type>::value;
  EXPECT_FALSE(r);
  r = contains_std_vector<StdVectorBuilder<true, double, double>::type>::value;
  EXPECT_FALSE(r);
}

TEST(MetaTraitsPrimArr, StdVectorBuilder_false_false) {
  using stan::StdVectorBuilder;
  using stan::length;
  using std::vector;

  std::vector<double> a_std_vector(3);

  StdVectorBuilder<false, double, double> dvv2(length(a_std_vector));
  EXPECT_THROW(dvv2[0], std::logic_error);
  EXPECT_THROW(dvv2.data(), std::logic_error);
}

TEST(MetaTraitsPrimArr, StdVectorBuilder_true_false) {
  using stan::StdVectorBuilder;
  using stan::length;
  using std::vector;

  std::vector<double> a_std_vector(3);

  StdVectorBuilder<true, double, double> dvv2(length(a_std_vector));
  EXPECT_FLOAT_EQ(0.0, dvv2[0]);
  EXPECT_FLOAT_EQ(0.0, dvv2[1]);
  EXPECT_FLOAT_EQ(0.0, dvv2[2]);
  double data2 = 0;
  EXPECT_NO_THROW(data2 = dvv2.data());
  EXPECT_FLOAT_EQ(0.0, data2);
}

TEST(MetaTraitsPrimArr, StdVectorBuilder_type_check) {
  using stan::StdVectorBuilder;
  using stan::contains_std_vector;

  bool r = contains_std_vector<
      StdVectorBuilder<true, double, std::vector<int>>::type>::value;
  EXPECT_TRUE(r);
  r = contains_std_vector<
      StdVectorBuilder<true, double, std::vector<double>>::type>::value;
  EXPECT_TRUE(r);
}

TEST(MetaTraitsPrimMat, StdVectorBuilder_false_false) {
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using stan::StdVectorBuilder;
  using stan::length;

  Matrix<double, Dynamic, 1> a_vector(4);
  Matrix<double, 1, Dynamic> a_row_vector(5);

  StdVectorBuilder<false, double, double> dvv3(length(a_vector));
  EXPECT_THROW(dvv3[0], std::logic_error);
  EXPECT_THROW(dvv3.data(), std::logic_error);

  StdVectorBuilder<false, double, double> dvv4(length(a_row_vector));
  EXPECT_THROW(dvv4[0], std::logic_error);
  EXPECT_THROW(dvv4.data(), std::logic_error);
}

TEST(MetaTraitsPrimMat, StdVectorBuilder_true_false) {
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using stan::StdVectorBuilder;
  using stan::length;

  Matrix<double, Dynamic, 1> a_vector(4);
  Matrix<double, 1, Dynamic> a_row_vector(5);

  StdVectorBuilder<true, double, double> dvv3(length(a_vector));
  EXPECT_FLOAT_EQ(0.0, dvv3[0]);
  EXPECT_FLOAT_EQ(0.0, dvv3[1]);
  EXPECT_FLOAT_EQ(0.0, dvv3[2]);
  double data3 = 0.0;
  EXPECT_NO_THROW(data3 = dvv3.data());
  EXPECT_FLOAT_EQ(0.0, data3);

  StdVectorBuilder<true, double, double> dvv4(length(a_row_vector));
  EXPECT_FLOAT_EQ(0.0, dvv4[0]);
  EXPECT_FLOAT_EQ(0.0, dvv4[1]);
  EXPECT_FLOAT_EQ(0.0, dvv4[2]);
  double data4 = 0.0;
  EXPECT_NO_THROW(data4 = dvv4.data());
  EXPECT_FLOAT_EQ(0.0, data4);
}

TEST(MetaTraitsPrimMat, StdVectorBuilder_type_check) {
  using stan::StdVectorBuilder;
  using stan::contains_std_vector;

  bool r = contains_std_vector<StdVectorBuilder<
      true, double, std::vector<Eigen::VectorXd>>::type>::value;
  EXPECT_TRUE(r);
  r = contains_std_vector<StdVectorBuilder<
      true, double, std::vector<Eigen::RowVectorXd>>::type>::value;
  EXPECT_TRUE(r);
  r = contains_std_vector<StdVectorBuilder<
      true, double,
      std::vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>>::
                              type>::value;
  EXPECT_TRUE(r);
  r = contains_std_vector<
      StdVectorBuilder<true, double, Eigen::VectorXd>::type>::value;
  EXPECT_FALSE(r);
  r = contains_std_vector<
      StdVectorBuilder<true, double, Eigen::RowVectorXd>::type>::value;
  EXPECT_FALSE(r);
  r = contains_std_vector<StdVectorBuilder<
      true, double,
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>>::type>::value;
  EXPECT_FALSE(r);
}
