cmake_minimum_required(VERSION 3.21)
project(alpha LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler

find_package(BISON 3.0 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

set(COMPILER_DIR ${CMAKE_SOURCE_DIR}/src/compiler)
set(COMPILER_GEN ${CMAKE_BINARY_DIR}/compiler)

BISON_TARGET(parser ${COMPILER_DIR}/parser.y ${COMPILER_GEN}/parser.c
  DEFINES_FILE ${COMPILER_GEN}/parser.h)
FLEX_TARGET(scanner ${COMPILER_DIR}/lex.l ${COMPILER_GEN}/scanner.c)
ADD_FLEX_BISON_DEPENDENCY(scanner parser)

add_executable(alpha-compiler
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
  ${COMPILER_DIR}/binary.c
  ${COMPILER_DIR}/consts.c
  ${COMPILER_DIR}/intermediate.c
  ${COMPILER_DIR}/stack.c
  ${COMPILER_DIR}/symbol_table.c
  ${COMPILER_DIR}/tools.c
)
target_include_directories(alpha-compiler PRIVATE ${COMPILER_DIR} ${COMPILER_GEN})
# Globals are defined in headers included by several units; -fno-common
# (the default since GCC 10 / Clang 11) turns that into link errors.
target_compile_options(alpha-compiler PRIVATE -fcommon)
target_link_libraries(alpha-compiler PRIVATE m)

# VM

set(VM_DIR ${CMAKE_SOURCE_DIR}/src/vm)

add_executable(alpha-vm
  ${VM_DIR}/AVM.cpp
  ${VM_DIR}/error.cpp
  ${VM_DIR}/execute.cpp
  ${VM_DIR}/lib.cpp
  ${VM_DIR}/mem.cpp
  ${VM_DIR}/tables.cpp
  ${VM_DIR}/vm.cpp
  ${VM_DIR}/vmarg.cpp
  ${VM_DIR}/execute/arithmetic.cpp
  ${VM_DIR}/execute/assign.cpp
  ${VM_DIR}/execute/boolean.cpp
  ${VM_DIR}/execute/branch.cpp
  ${VM_DIR}/execute/func.cpp
  ${VM_DIR}/execute/nop.cpp
  ${VM_DIR}/execute/table.cpp
  ${VM_DIR}/tools/arith.cpp
  ${VM_DIR}/tools/bool.cpp
  ${VM_DIR}/tools/strings.cpp
)
target_include_directories(alpha-vm PRIVATE ${VM_DIR})

# Tests

include(CTest)
if(BUILD_TESTING)
  set(TEST_TIMEOUT 10)
  set(TEST_OUT ${CMAKE_BINARY_DIR}/tests)
  file(MAKE_DIRECTORY ${TEST_OUT})

  # Front-end tests: Error*.asc must be rejected, everything else must compile.
  file(GLOB compiler_tests CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/compiler/*.asc)
  foreach(src IN LISTS compiler_tests)
    get_filename_component(name ${src} NAME_WE)
    add_test(NAME compiler/${name}
      COMMAND alpha-compiler ${src} ${TEST_OUT}/compiler_${name}.abc)
    set_tests_properties(compiler/${name} PROPERTIES TIMEOUT ${TEST_TIMEOUT})
    if(name MATCHES "^Error")
      set_tests_properties(compiler/${name} PROPERTIES WILL_FAIL TRUE)
    endif()
  endforeach()

  # End-to-end tests: compile to bytecode, then run it on the VM. The err*
  # programs raise runtime errors, which the VM reports without crashing.
  file(GLOB vm_tests CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/vm/*.asc)
  foreach(src IN LISTS vm_tests)
    get_filename_component(name ${src} NAME_WE)
    set(abc ${TEST_OUT}/vm_${name}.abc)
    add_test(NAME vm/${name}/compile COMMAND alpha-compiler ${src} ${abc})
    add_test(NAME vm/${name}/run COMMAND alpha-vm ${abc})
    set_tests_properties(vm/${name}/compile PROPERTIES
      TIMEOUT ${TEST_TIMEOUT} FIXTURES_SETUP vm_${name})
    set_tests_properties(vm/${name}/run PROPERTIES
      TIMEOUT ${TEST_TIMEOUT} FIXTURES_REQUIRED vm_${name})
  endforeach()
endif()
