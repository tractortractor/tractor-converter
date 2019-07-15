INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/volInt)



set(BOOST_MIN_VERSION "1.60.0")

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
if(WIN32)
  add_definitions(-DBOOST_ALL_NO_LIB)
  # Must be used or MSVC 2017 will link program_options AND libprogram_options
  # even when asked to link ONLY program_options.
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif(WIN32)
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED)



SET(VOLINT_SOURCES
  volInt.cpp
  )

SET(VOLINT_HEADER_FILES
  volInt.hpp
  )

ADD_LIBRARY(volInt STATIC
  ${VOLINT_SOURCES}
  ${VOLINT_HEADER_FILES}
  )



# include dirs
target_include_directories(volInt SYSTEM
  PRIVATE ${Boost_INCLUDE_DIRS}
  )
