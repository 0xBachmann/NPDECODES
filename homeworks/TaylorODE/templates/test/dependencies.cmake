# Add your custom dependencies here:

# DIR will be provided by the calling file.

set(SOURCES
  ${DIR}/taylorode.h
  ${DIR}/taylorode.cc
  ${DIR}/test/taylorode_test.cc
)


set(LIBRARIES
  Eigen3::Eigen
  GTest::gtest_main
)
