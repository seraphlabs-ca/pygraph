INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../../graph/include)

##############################################################################
# HDF5
##############################################################################
FIND_PACKAGE(HDF5 1.8.0)
INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})


#############################################################
# pygraph
#############################################################


# files 
SET(PYGRAPH_FILES
        pygraph.cpp pygraph.h
    )

SET(PYGRAPH_LIBRARIES
    ${HDF5_LIBRARIES}
  )

# pygraph library
BUILD_LIBRARY(pygraph "${PYGRAPH_FILES}" "${PYGRAPH_LIBRARIES}")

# pygraph SWIG library
SWIG_BUILD_MODULE_PY(pygraphSWIG pygraphSWIG.i "pygraph")
