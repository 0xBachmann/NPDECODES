#if SOLUTION
# Dependencies of mastersolution tests:
#else
# Add your custom dependencies here:
#endif

# DIR will be provided by the calling file.

set(SOURCES
  ${DIR}/test/trans_gal_mat_test.cc
)

set(LIBRARIES
  Eigen3::Eigen
  GTest::gtest_main
)