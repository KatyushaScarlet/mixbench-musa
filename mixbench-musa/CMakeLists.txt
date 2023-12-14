cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_C_COMPILER "mcc")
set(CMAKE_CXX_COMPILER "mcc")
set(CMAKE_CUDA_COMPILER "mcc")

project(mixbench LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CXX_NORMAL_FLAGS
  "-fPIC"
  )

set(CXX_WARN_FLAGS
  "-Wno-return-type -Wno-unused-function -Wno-deprecated-declarations -Wno-format-security -Wno-deprecated-register -Wno-pointer-bool-conversion"
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_NORMAL_FLAGS} ${CXX_WARN_FLAGS}")

set(CMAKE_CUDA_FLAGS "-mtgpu")

set(EXTRA_CUDA_FLAGS "" CACHE STRING "Extra compiler flags for CUDA")

if(EXTRA_CUDA_FLAGS)
  set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${EXTRA_CUDA_FLAGS}")
endif()

set(MUSA_INSTALL_PATH "/usr/local/musa")
set(RT_LIB_NAME "musart")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${MUSA_INSTALL_PATH}/include
)

link_directories(${MUSA_INSTALL_PATH}/lib)
set(BASIC_LIB -l${RT_LIB_NAME})

# Include CUDA header directory in cpp files
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

# Get version info from git tag
execute_process(COMMAND git describe --tags --always
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "")
endif()
string(STRIP "${GIT_REV}" GIT_REV)
file(WRITE "version_info.h" "#define VERSION_INFO \"")
file(APPEND "version_info.h" ${GIT_REV})
file(APPEND "version_info.h" "\"")

list(APPEND CMAKE_MODULE_PATH /usr/local/musa/cmake)
find_package(MUSA REQUIRED)

musa_add_executable(mixbench-cuda main-cuda.cpp mix_kernels_cuda.h lcutil.h version_info.h mix_kernels_cuda.cu mix_kernels_cuda.h lcutil.h)
target_link_libraries(mixbench-cuda
    ${BASIC_LIB}
  )
target_compile_features(mixbench-cuda PUBLIC cxx_std_11)
