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
option(LIB_NVM_CORE_USE_TEST ON)

set(LIB_NVM_CORE_BUILD_DOC OFF)
set(LIB_NVM_CORE_SANITIZE_ADDRESS ON)
set(LIB_NVM_CORE_USE_CATCH ON)
set(LIB_NVM_CORE_USE_LIB ON)
set(LIB_NVM_CORE_USE_TEST ON)


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

# UTF8CPP
file(GLOB_RECURSE SOURCES_UTF8CPP CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/utfcpp/source/*.h
)

add_library(utf8cpp ${SOURCES_UTF8CPP})
set_target_properties(utf8cpp PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(utf8cpp PUBLIC cxx_std_17)
target_include_directories(utf8cpp
    PUBLIC
        deps/utfcpp/source/
)

# HH Date
option( USE_SYSTEM_TZ_DB "Use the operating system's timezone database" OFF )
option( MANUAL_TZ_DB "User will set TZ DB manually by invoking set_install in their code" OFF )
option( USE_TZ_DB_IN_DOT "Save the timezone database in the current folder" OFF )
option( BUILD_SHARED_LIBS  "Build a shared version of library" OFF )
option( ENABLE_DATE_TESTING "Enable unit tests" OFF )
option( DISABLE_STRING_VIEW "Disable string view" OFF )
option( COMPILE_WITH_C_LOCALE "define ONLY_C_LOCALE=1" OFF )
option( BUILD_TZ_LIB "build/install of TZ library" OFF )



set( USE_SYSTEM_TZ_DB  ON )
set( MANUAL_TZ_DB OFF )
set( USE_TZ_DB_IN_DOT  OFF )
set( BUILD_SHARED_LIBS  OFF )
set( ENABLE_DATE_TESTING  OFF )
set( DISABLE_STRING_VIEW  ON )
set( COMPILE_WITH_C_LOCALE  OFF )
set( BUILD_TZ_LIB  OFF )

# add_subdirectory(deps/hhdate build-hhdate)

# SET(USE_SYSTEM_TZ_DB ON)
# SET(DISABLE_STRING_VIEW ON)
# SET(BUILD_TZ_LIB OFF)

# Main headers and sources
file(GLOB_RECURSE SOURCES_NVM_CORE CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/hhdate/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/hhdate/src/*.cpp
)



add_library(${PROJECT_NAME} STATIC ${SOURCES_NVM_CORE} )
target_link_libraries(${PROJECT_NAME}  
    utf8cpp )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if( USE_SYSTEM_TZ_DB OR MANUAL_TZ_DB )
    target_compile_definitions( ${PROJECT_NAME}  PRIVATE AUTO_DOWNLOAD=0 HAS_REMOTE_API=0 )
else()
    target_compile_definitions( ${PROJECT_NAME}  AUTO_DOWNLOAD=1 HAS_REMOTE_API=1 )
endif()

if ( USE_SYSTEM_TZ_DB AND NOT WIN32 AND NOT MANUAL_TZ_DB )
    target_compile_definitions( ${PROJECT_NAME} PRIVATE INSTALL=. PUBLIC USE_OS_TZDB=1 )
else()
    target_compile_definitions( ${PROJECT_NAME} PUBLIC USE_OS_TZDB=0 )
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        inc/
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/hhdate/include/
)


add_executable(${PROJECT_NAME}-test main.cc)
if(Sanitizers_FOUND)
    add_sanitizers("${PROJECT_NAME}-test")
    message(STATUS "${PROJECT_NAME}-test_LIB::ASAN:ON.")
endif()

target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME} )
target_compile_features(${PROJECT_NAME}-test PUBLIC cxx_std_17)
target_include_directories("${PROJECT_NAME}-test"
    PUBLIC
        inc/
)

if(LIB_NVM_CORE_USE_TEST)
    message(STATUS "TEST::ADD_UNIT_TEST.")
    add_subdirectory(tests/nvm)
endif()       

message(STATUS "NVM::Core LIB Configuration Done!\n")
set(nvm_core_FOUND  ON)