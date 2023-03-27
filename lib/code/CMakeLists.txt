# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

find_package(FLEX 2.6)
find_package(BISON 3.8)

set(GENERATED_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

add_library(spreader STATIC)

set_target_properties(spreader PROPERTIES
    XCODE_GENERATE_SCHEME true
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    C_STANDARD 17
    CXX_STANDARD_REQUIRED True
    C_STANDARD_REQUIRED True
)

target_link_libraries(spreader 
PUBLIC
    intrusive-shared-ptr
    sys_string
)

target_include_directories(spreader
PUBLIC
    inc
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${GENERATED_DIR}
    $<$<BOOL:${SKBUILD}>:${Python3_INCLUDE_DIR}>
)

target_compile_definitions(spreader
PUBLIC
    $<$<BOOL:${SKBUILD}>:SYS_STRING_USE_PYTHON=1>
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:YY_NO_UNISTD_H>
)


target_compile_options(spreader 
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8;/W4;/WX;/wd4065;/wd4505;/wd4324;/permissive-;/Zc:__cplusplus,throwingNew,preprocessor>
    $<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-pedantic;-ftemplate-backtrace-limit=0>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall;-Wextra;-pedantic;-fpch-instantiate-templates;-ftemplate-backtrace-limit=0>
    $<$<CXX_COMPILER_ID:GNU>:-Wall;-Wextra;-pedantic> 


    $<$<AND:$<BOOL:${SPR_ENABLE_COVERAGE}>,$<CXX_COMPILER_ID:AppleClang>>:-fprofile-instr-generate -fcoverage-mapping>
)

target_link_options(spreader
PRIVATE
    $<$<AND:$<BOOL:${SPR_ENABLE_COVERAGE}>,$<CXX_COMPILER_ID:AppleClang>>:-fprofile-instr-generate -fcoverage-mapping>
)


if (BISON_FOUND AND FLEX_FOUND)

    file(RELATIVE_PATH RELATIVE_SOURCE_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    file(RELATIVE_PATH RELATIVE_BINARY_DIR ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

    BISON_TARGET(FormulaParser src/formula.y ${GENERATED_DIR}/formula-parser.cpp 
        DEFINES_FILE ${GENERATED_DIR}/formula-parser.h 
        COMPILE_FLAGS "-Wcounterexamples -Wdangling-alias -Wdeprecated -Wprecedence --file-prefix-map=${CMAKE_CURRENT_LIST_DIR}=${RELATIVE_SOURCE_DIR} --file-prefix-map=${CMAKE_CURRENT_BINARY_DIR}=${RELATIVE_BINARY_DIR}"
    )
    FLEX_TARGET(FormulaLexer src/formula.l ${GENERATED_DIR}/formula-lexer.cpp 
        DEFINES_FILE ${GENERATED_DIR}/formula-lexer.h 
        COMPILE_FLAGS "--never-interactive -L"
    )
    ADD_FLEX_BISON_DEPENDENCY(FormulaLexer FormulaParser)

    set(GENERATED_SOURCES
        ${BISON_FormulaParser_OUTPUTS}
        ${FLEX_FormulaLexer_OUTPUTS}
    )

else()

    set(GENERATED_SOURCES
        ${GENERATED_DIR}/formula-parser.cpp 
        ${GENERATED_DIR}/formula-parser.h 
        ${GENERATED_DIR}/formula-lexer.cpp 
        ${GENERATED_DIR}/formula-lexer.h 
    )

endif()


set(PUBLIC_HEADERS
    inc/spreader/array.h
    inc/spreader/ast-node.h
    inc/spreader/cell.h
    inc/spreader/cell-grid.h
    inc/spreader/coerce.h
    inc/spreader/compiler.h
    inc/spreader/coro-generator.h
    inc/spreader/coro-async.h
    inc/spreader/error.h
    inc/spreader/error-handling.h
    inc/spreader/externals.h
    inc/spreader/fixed-size-memory-resource.h
    inc/spreader/floating-decimal.h
    inc/spreader/formula.h
    inc/spreader/formula-references.h
    inc/spreader/geometry.h
    inc/spreader/interval-map.h
    inc/spreader/iteration.h
    inc/spreader/linked-list.h
    inc/spreader/mru-cache.h
    inc/spreader/name-manager.h
    inc/spreader/number.h
    inc/spreader/numeric.h
    inc/spreader/reference.h
    inc/spreader/scalar.h
    inc/spreader/scalar-generator.h
    inc/spreader/sheet.h
    inc/spreader/stack-memory-resource.h
    inc/spreader/tree-traversal.h
    inc/spreader/typelist.h
    inc/spreader/types.h
    inc/spreader/util.h
)
source_group("Public Headers" FILES ${PUBLIC_HEADERS})

set(SOURCES
    src/ast-node.cpp
    src/cell.cpp
    src/cell-grid.cpp
    src/coerce.cpp
    src/error.cpp
    src/execution-context.h
    src/floating-decimal.cpp
    src/formula-parser-builder.h
    src/formula.cpp
    src/formula-evaluator.h
    src/formula-evaluator.cpp
    src/formula-references.cpp
    src/formula.l
    src/formula.y
    src/macro-map.h
    src/mini-trie.h
    src/name-manager.cpp
    src/number.cpp
    src/number-matcher.h
    src/scalar-xif-matcher.h
    src/scalar-xif-matcher.cpp
    src/sheet.cpp
    src/whitespace.h
    src/wildcarder.h
)
source_group("Sources" FILES ${SOURCES})

set(SCALAR_FUNCTIONS
    src/scalar-aggregators.h
    src/scalar-boolean-aggregators.h
    src/scalar-boolean-functions.h
    src/scalar-date-functions.h
    src/scalar-function.h
    src/scalar-info-functions.h
    src/scalar-lookup.h
    src/scalar-numeric-aggregators.h
    src/scalar-numeric-functions.h
    src/scalar-operators.h
    src/scalar-string-aggregators.h
    src/scalar-string-functions.h
)
source_group("Scalar Functions" FILES ${SCALAR_FUNCTIONS})

set(FUNCTIONS
    src/functions/true-function.h
    src/functions/true-function.cpp
    src/functions/true-function-creation.h
    
    src/functions/unary-operators.h
    src/functions/unary-operators.cpp
    src/functions/binary-operators.h
    src/functions/binary-operators.cpp

    src/functions/aggregator-function.cpp
    src/functions/aggregator-if-function.cpp
    src/functions/finite-scalar-function.cpp

    src/functions/func-choose.cpp
    src/functions/func-concatenate.cpp
    src/functions/func-hvlookup.cpp
    src/functions/func-if.cpp
    src/functions/func-index.cpp
    src/functions/func-indirect.cpp
    src/functions/func-match.cpp
    src/functions/func-switch.cpp
    src/functions/func-row-column.cpp
    src/functions/func-transpose.cpp
)
source_group("Functions" FILES ${FUNCTIONS})

source_group("Generated Sources" FILES ${GENERATED_SOURCES})

target_sources(spreader 
PRIVATE
    ${PUBLIC_HEADERS}
    ${SOURCES}
    ${FUNCTIONS}
    ${SCALAR_FUNCTIONS}
    ${GENERATED_SOURCES}
)
