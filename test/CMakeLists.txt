# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

set(GTEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/gtest")
set(GTEST_BINARY_DIR "${GTEST_DIR}/bin")

set(GMOCK_LIB_PATH "${GTEST_BINARY_DIR}/lib/libgmock.a")
set(GTEST_LIB_PATH "${GTEST_BINARY_DIR}/lib/libgtest.a")

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${GTEST_DIR}
    BINARY_DIR ${GTEST_BINARY_DIR}
    BUILD_BYPRODUCTS ${GMOCK_LIB_PATH} ${GTEST_LIB_PATH}
    # Disable install step
    INSTALL_COMMAND ""
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest STATIC IMPORTED)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${GTEST_LIB_PATH}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
add_dependencies(libgtest gtest)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock STATIC IMPORTED)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${GMOCK_LIB_PATH}"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
add_dependencies(libgmock gtest)

include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include")

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

add_executable(tests ${SRC})
add_dependencies(tests gtest)
target_link_libraries(tests LINK_PUBLIC
    SCONE
    libgtest
    libgmock
)
