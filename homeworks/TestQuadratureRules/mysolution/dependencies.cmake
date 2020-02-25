# Add your custom dependencies here:

# DIR will be provided by the calling file.

set(SOURCES
  ${DIR}/testquadraturerules_main.cc
  ${DIR}/testquadraturerules.h
  ${DIR}/testquadraturerules.cc
)

set(LIBRARIES
  Eigen3::Eigen
  LF::lf.base
  LF::lf.quad
)
