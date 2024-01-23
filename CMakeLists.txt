cmake_minimum_required(VERSION 3.10)
project(nvm-core CXX)

message( "NVM::NVM CORE PLUGINS v2")
message( "-----------------------------")
message(STATUS "NVM::NVM_CORE LIB DIR:${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(ABSL_PROPAGATE_CXX_STD ON)

option(LIB_NVM_CORE_BUILD_DOC "Build documentation" OFF)
option(LIB_NVM_CORE_SANITIZE_ADDRESS "SANITIZE ADDRESS" ON)
option(LIB_NVM_CORE_USE_CATCH ON)
option(LIB_NVM_CORE_USE_LIB ON)
option(LIB_NVM_CORE_USE_TEST OFF)

# Add ASAN
if(LIB_NVM_CORE_SANITIZE_ADDRESS)
    set(SANITIZE_ADDRESS ON)
endif(LIB_NVM_CORE_SANITIZE_ADDRESS)

#  make ourself sanitize and sane well.
if(NOT Sanitizers_FOUND)
 find_package(Sanitizers)
endif()

# Add Doxygen
if(LIB_NVM_CORE_BUILD_DOC)
    if(NOT DOXYGEN_FOUND)
        find_package(Doxygen)
    endif()

    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/docs-cached/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

         # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_nvmcore ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/generated/docs-cached/
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(LIB_NVM_CORE_BUILD_DOC)

# Add Unit Test Lib Catch2
if(LIB_NVM_CORE_USE_CATCH)
 message(STATUS  "CATCH2::ADD_LIB.")
 find_package(Catch2 3 REQUIRED)
endif()

# Add dependencies source code

# Main headers and sources
file(GLOB_RECURSE SOURCES_NVM_CORE CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)

add_library(${PROJECT_NAME} ${SOURCES_NVM_CORE} )
target_link_libraries(${PROJECT_NAME} utf8cpp )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        inc/
)


add_executable(${PROJECT_NAME}-test main.cc)
if(Sanitizers_FOUND)
    add_sanitizers("${PROJECT_NAME}-test")
    message(STATUS "${PROJECT_NAME}-test_LIB::ASAN:ON.")
endif()

target_link_libraries(${PROJECT_NAME}-test PUBLIC ${PROJECT_NAME} )
target_include_directories("${PROJECT_NAME}-test"
    PUBLIC
        inc/
)

if(LIB_NVM_CORE_USE_TEST)
    message(STATUS "TEST::ADD_UNIT_TEST.")
    add_subdirectory(tests)
endif()       

message(STATUS "Nava::Core LIB Configuration Done!\n")
set(nvm_core_FOUND  ON)