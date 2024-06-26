cmake_minimum_required(VERSION 3.10)
project(nvmcore-unit-test CXX)

# Set the path to the directory containing the Catch2 header files

# Add the Catch2 library
# add_library(Catch2 INTERFACE)
# target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})

# Set the path to the directory containing your test source files
set(TEST_SOURCES

    byte_converter_decode_test.cc
    byte_converter_encode_test_big_endian.cc
    byte_converter_encode_test_little_endian.cc
    logic_test.cc
    datetime_test.cc
    record_test.cc
    validation_test.cc
    select_test.cc
    struct_mapper_test.cc
    nvsql-policy/order_by_policy_test.cc
    nvasync/nv_async_test.cc
    type_utility_test.cc
    # Add more test files if needed
)

if(NOT Catch2_FOUND)
    message(STATUS "CATCH2::ADD_LIB.")
    find_package(Catch2 3 REQUIRED)
endif()


# Create an executable for the tests
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

# Add Include location
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

# Link the Catch2 library to the test executable
target_link_libraries(${PROJECT_NAME} PUBLIC Catch2::Catch2WithMain nvcore)

# Set the C++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)