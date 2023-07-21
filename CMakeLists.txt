cmake_minimum_required(VERSION 3.20)

project(BERNpp5)

set(CMAKE_CXX_STANDARD 14)
set(USE_SWIG Off)
add_library(libBERN5 STATIC BERNpp/Community.cpp BERNpp/DataAccess.cpp BERNpp/SiteVector.cpp BERNpp/species.cpp)
add_executable(BERNpp5 main.cpp)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(libBERN5 PUBLIC OpenMP::OpenMP_CXX)
    message("Using OpenMP")
endif()
target_link_libraries(BERNpp5 libBERN5)

if (USE_SWIG)

    #Find the environment variable for PYTHONHOME
    find_package (Python3 COMPONENTS Interpreter Development)
    include_directories(${Python3_INCLUDE_DIRS})
    link_libraries(${Python3_LIBRARIES})

    #Specify the directory of your. cxx and other files
    include_directories(${PROJECT_SOURCE_DIR}/BERNpp)

    #Looking for the installed Swig is actually going to the computer to find the Swig environment you installed, so we need to install the environment in advance.
    find_package(SWIG REQUIRED)
    include(${SWIG_USE_FILE})
    #Python file output directory - outdir
    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/python)
    #Specify an output directory name to place the generated source files. For example, output exampleptyhon_ The location of the wrap.cxx file, etc
    set(SWIG_OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cpp)
    #Start build
    swig_add_library(bern LANGUAGE python SOURCES BERNpp/bern.i)
    swig_link_libraries(bern libBERN5)

endif()