# Dependencies of mastersolution:

# DIR will be provided by the calling file.

set(SOURCES
  ${DIR}/exponentialintegrator.h
  ${DIR}/exponentialintegrator.cc
  ${DIR}/test/exponentialintegrator_test.cc
)


set(LIBRARIES
  Eigen3::Eigen
  GTest::gtest_main
)
