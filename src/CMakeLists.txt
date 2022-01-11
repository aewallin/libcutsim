project(libcutsim)

cmake_minimum_required(VERSION 2.4)

#
# Turn compiler warnings up to 11, at least with gcc.  I dont know how to
# do this with other compilers we might support and I'm leaving it up to
# the relevant platform maintainers...
# #include'ing the boost graph-library creates deprecated warnings
# thus we use now use -Wno-deprecated here.
#
if (CMAKE_BUILD_TOOL MATCHES "make")
    MESSAGE(STATUS "setting gcc options: -Wall -Werror -Wno-deprecated -pedantic-errors")
    add_definitions(-Wall  -Wno-deprecated -Werror -pedantic-errors -Wno-error=multichar )
    add_definitions(-fPIC)
    add_definitions(-std=c++17) 
endif (CMAKE_BUILD_TOOL MATCHES "make")

option(BUILD_TYPE
  "Build type: Release=ON/Debug=OFF  " ON)
  #"Build type: Release=ON/Debug=OFF  " OFF)

if (BUILD_TYPE)
    MESSAGE(STATUS " CMAKE_BUILD_TYPE = Release")
    set(CMAKE_BUILD_TYPE Release)
endif(BUILD_TYPE)

if (NOT BUILD_TYPE)
    MESSAGE(STATUS " CMAKE_BUILD_TYPE = Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT BUILD_TYPE)

# find BOOST and boost-python
find_package( Boost  COMPONENTS python REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "found Boost: " ${Boost_LIB_VERSION})
    MESSAGE(STATUS "boost-incude dirs are: " ${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "boost_LIBRARY_DIRS is: " ${Boost_LIBRARY_DIRS})
    MESSAGE(STATUS "Boost_LIBRARIES is: " ${Boost_LIBRARIES})    
endif()

# this figures out the Python include directories and adds them to the
# header file search path
execute_process(
    COMMAND python3-config --includes
    COMMAND sed -r "s/-I//g; s/ +/;/g"
    COMMAND tr -d '\n'
    OUTPUT_VARIABLE Python_Includes
)
include_directories(${Python_Includes})
    
# this defines the source-files

MESSAGE(STATUS "CMAKE_SOURCE_DIR = " ${CMAKE_SOURCE_DIR} )

set( CUTSIM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/volume.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/octnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/octree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/marching_cubes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gldata.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/bbox.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/cutsim.cpp 
)

set( CUTSIM_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/facet.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/octnode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/octree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/volume.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bbox.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/isosurface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/marching_cubes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cube_wireframe.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gldata.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glvertex.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cutsim.hpp 
)


# static library, linked with the python module below
add_library(
    libcutsim_static
    STATIC
    ${CUTSIM_SRC}
)

set_target_properties(libcutsim_static PROPERTIES PREFIX "") # avoid liblib

# this makes the Python module
add_library(
    libcutsim
    MODULE
    cutsim_py.cpp
)
target_link_libraries(libcutsim libcutsim_static ${Boost_LIBRARIES} ) 
set_target_properties(libcutsim PROPERTIES PREFIX "") # avoid liblib



# this figures out where to install the Python modules
execute_process(
    COMMAND python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_VARIABLE Python_site_packages
    OUTPUT_STRIP_TRAILING_WHITESPACE
) # on Ubuntu 11.10 this outputs: /usr/local/lib/python2.7/dist-packages

# strip away /usr/local/  because that is what CMAKE_INSTALL_PREFIX is set to
# also, since there is no leading "/", it makes ${Python_site_packages} a relative path.
STRING(REGEX REPLACE "/usr/local/(.*)$" "\\1" Python_site_packages "${Python_site_packages}" )

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is : " ${CMAKE_INSTALL_PREFIX})
MESSAGE(STATUS "Python libraries will be installed to: " ${Python_site_packages})

# this installs the python library
install(
    TARGETS libcutsim
    LIBRARY DESTINATION ${Python_site_packages}
)
