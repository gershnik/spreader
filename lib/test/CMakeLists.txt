add_executable(library-test)

set_target_properties(library-test PROPERTIES
    XCODE_GENERATE_SCHEME true
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    C_STANDARD 17
    CXX_STANDARD_REQUIRED True
    C_STANDARD_REQUIRED True
)

target_link_libraries(library-test
PRIVATE
    spreader
    Catch2::Catch2
)

target_compile_options(library-test
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8;/W4;/WX;/permissive-;/Zc:throwingNew>
    $<$<AND:$<BOOL:${SPR_ENABLE_COVERAGE}>,$<CXX_COMPILER_ID:AppleClang>>:-fprofile-instr-generate -fcoverage-mapping>
    #$<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:AppleClang>>:-fsanitize=address,undefined>
)

target_compile_definitions(library-test
PRIVATE
    SPR_TESTING=1
)

if(DEFINED EMSCRIPTEN)
    target_link_options(library-test
    PRIVATE
        #-sWASM=0
        -sASSERTIONS
        -sALLOW_MEMORY_GROWTH
    )
else()
    target_link_options(library-test
    PRIVATE
        $<$<AND:$<BOOL:${SPR_ENABLE_COVERAGE}>,$<CXX_COMPILER_ID:AppleClang>>:-fprofile-instr-generate -fcoverage-mapping>
        #$<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:AppleClang>>:-fsanitize=address,undefined>
    )
endif()

target_sources(library-test
PRIVATE
    test-util.h
    main.cpp
    test-cell-grid.cpp
    test-coercion.cpp
    test-formula-parser.cpp
    test-floating-decimal.cpp
    
    test-formula-address.cpp
    test-formula-and-or-xor-not.cpp
    test-formula-average.cpp
    test-formula-ceil-floor.cpp
    test-formula-choose.cpp
    test-formula-concatenate.cpp
    test-formula-count.cpp
    test-formula-date.cpp
    test-formula-datedif.cpp
    test-formula-days.cpp
    test-formula-edate-eomonth.cpp
    test-formula-find.cpp
    test-formula-hlookup.cpp
    test-formula-index.cpp
    test-formula-indirect.cpp
    test-formula-int-mod.cpp
    test-formula-is-errors.cpp
    test-formula-left-right.cpp
    test-formula-len.cpp
    test-formula-lower-upper.cpp
    test-formula-max-min.cpp
    test-formula-match.cpp
    test-formula-math-op.cpp
    test-formula-mid-replace.cpp
    test-formula-now-today.cpp
    test-formula-row-column.cpp
    test-formula-round.cpp
    test-formula-sign.cpp
    test-formula-stdev.cpp
    test-formula-substitute.cpp
    test-formula-sum.cpp
    test-formula-sumif.cpp
    test-formula-switch.cpp
    test-formula-time.cpp
    test-formula-transpose.cpp
    test-formula-trim.cpp
    test-formula-vlookup.cpp
    test-formula-weekday.cpp
    test-formula-weeknum.cpp

    test-interval-map.cpp
    test-mini-trie.cpp
    test-number-matcher.cpp
    test-scalar-math.cpp
    test-sheet.cpp
    test-sheet-parsing.cpp
    test-sheet-copy-move.cpp
    test-sheet-insert-delete.cpp
    test-sheet-length-info.cpp
    test-sheet-funky.cpp
    test-xif-matcher.cpp
    test-wildcarder.cpp
)

set(TEST_COMMAND "")
set(TEST_DEPS "")

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND SPR_ENABLE_COVERAGE)

    list(APPEND TEST_COMMAND COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=$<TARGET_FILE_DIR:library-test>/test.profraw $<TARGET_FILE:library-test>)
    list(APPEND TEST_COMMAND COMMAND xcrun llvm-profdata merge -sparse $<TARGET_FILE_DIR:library-test>/test.profraw -o $<TARGET_FILE_DIR:library-test>/test.profdata)
    list(APPEND TEST_COMMAND COMMAND xcrun llvm-cov show -format=html 
                -arch `uname -m`
                -Xdemangler=c++filt -Xdemangler -n
                -show-regions=1
                -show-instantiations=0
                #-show-branches=count
                #-show-instantiation-summary=1
                -ignore-filename-regex=test/.\\*
                -ignore-filename-regex=.\\*/_deps/.\\*
                -output-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage 
                -instr-profile=$<TARGET_FILE_DIR:library-test>/test.profdata 
                $<TARGET_FILE:library-test>)

elseif(DEFINED CMAKE_CROSSCOMPILING_EMULATOR)
    list(APPEND TEST_COMMAND COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:library-test>)
else()
    list(APPEND TEST_COMMAND COMMAND $<TARGET_FILE:library-test>)
endif()

list(APPEND TEST_DEPS library-test)

add_custom_target(run-library-test
    DEPENDS ${TEST_DEPS}
    ${TEST_COMMAND}
)
