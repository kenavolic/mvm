# Minimum CMake required
cmake_minimum_required(VERSION 3.9)

# Project
project(mvm LANGUAGES CXX VERSION 0.2.0)

# Options
option(MVM_BUILD_TESTS "Build tests" ON)
option(MVM_BUILD_EXAMPLES "Build examples" ON)
option(MVM_BUILD_WITH_TRACES "Build with traces" OFF)
option(MVM_BUILD_WITH_FAST_INSTR "Build with fast instruction handling for best performance" ON)

# General Config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compilation flags
if (MSVC)
    set(MVM_TEST_COMPILE_OPTIONS_DEBUG "/MTd")
    set(MVM_TEST_COMPILE_OPTIONS_RELEASE "/MT")
endif()

# Dependencies
add_subdirectory(third_party)

# Lib
set (MVM_LIB mvm)

set (MVM_CORE_INCL
    ${PROJECT_SOURCE_DIR}/include/mvm/assembler.h
    ${PROJECT_SOURCE_DIR}/include/mvm/bytecode_serializer.h
    ${PROJECT_SOURCE_DIR}/include/mvm/concept.h
    ${PROJECT_SOURCE_DIR}/include/mvm/disassembler.h
    ${PROJECT_SOURCE_DIR}/include/mvm/except.h
    ${PROJECT_SOURCE_DIR}/include/mvm/instr_set.h
    ${PROJECT_SOURCE_DIR}/include/mvm/interpreter.h
    ${PROJECT_SOURCE_DIR}/include/mvm/macros.h
    ${PROJECT_SOURCE_DIR}/include/mvm/meta.h
    ${PROJECT_SOURCE_DIR}/include/mvm/mvm.h
    ${PROJECT_SOURCE_DIR}/include/mvm/program.h
    ${PROJECT_SOURCE_DIR}/include/mvm/status.h
    ${PROJECT_SOURCE_DIR}/include/mvm/traits.h
    ${PROJECT_SOURCE_DIR}/include/mvm/trace.h
    ${PROJECT_SOURCE_DIR}/include/mvm/types.h
    ${PROJECT_SOURCE_DIR}/include/mvm/value_stack.h
    ${PROJECT_SOURCE_DIR}/include/mvm/vm.h    
)

set (MVM_HELPERS_INCL
    ${PROJECT_SOURCE_DIR}/include/mvm/helpers/list.h
    ${PROJECT_SOURCE_DIR}/include/mvm/helpers/num_parse.h
    ${PROJECT_SOURCE_DIR}/include/mvm/helpers/reflect.h
	${PROJECT_SOURCE_DIR}/include/mvm/helpers/typestring.h
    ${PROJECT_SOURCE_DIR}/include/mvm/helpers/utils.h
)

set (MVM_INCL ${MVM_CORE_INCL} ${MVM_HELPERS_INCL})

add_library(${MVM_LIB} INTERFACE)
target_include_directories(
    ${MVM_LIB} INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(${MVM_LIB} INTERFACE cxx_std_17)

if (MVM_BUILD_WITH_TRACES)
    target_compile_definitions(${MVM_LIB} INTERFACE ENABLE_TRACES)
endif()

if (MVM_BUILD_WITH_FAST_INSTR)
    target_compile_definitions(${MVM_LIB} INTERFACE FASTI)
endif()

if (MSVC)
    add_custom_target(${MVM_LIB}_headers SOURCES ${MVM_INCL})
    set_target_properties(${MVM_LIB}_headers PROPERTIES FOLDER "lib")
endif()

# Tests
if (MVM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if (MVM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/MvmConfigVersion.cmake" 
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(TARGETS ${MVM_LIB} 
    EXPORT MvmTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/MvmConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/MvmConfig.cmake"
    INSTALL_DESTINATION cmake/
)
install(EXPORT MvmTargets NAMESPACE mvm:: DESTINATION cmake/)
install(FILES "${PROJECT_BINARY_DIR}/MvmConfig.cmake" "${PROJECT_BINARY_DIR}/MvmConfigVersion.cmake" DESTINATION cmake/)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION .)

# Summary
message(STATUS "Configuration summary")
message(STATUS "-- Project name                 : ${PROJECT_NAME}")
message(STATUS "-- Project version              : ${PROJECT_VERSION}")
message(STATUS "-- Build examples               : ${MVM_BUILD_EXAMPLES}")
message(STATUS "-- Build tests                  : ${MVM_BUILD_TESTS}")
message(STATUS "-- Build with traces            : ${MVM_BUILD_WITH_TRACES}")
message(STATUS "-- Build with fast instr        : ${MVM_BUILD_WITH_FAST_INSTR}")
message(STATUS "-- Install dir                  : ${CMAKE_INSTALL_PREFIX}")
