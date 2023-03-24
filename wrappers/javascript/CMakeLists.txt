# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

configure_file(package.json.config package.json @ONLY)

add_executable(spreader.js)

set_target_properties(spreader.js PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    C_STANDARD 17
    CXX_STANDARD_REQUIRED True
    C_STANDARD_REQUIRED True

    OUTPUT_NAME spreader
)

target_link_libraries(spreader.js
PRIVATE
    spreader
    embind
)

target_include_directories(spreader.js
PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_options(spreader.js
PRIVATE
    #-sWASM=0
    -sEXPORT_NAME='Spreader'
    -sMODULARIZE=1
    -sNODEJS_CATCH_EXIT=0
    -sNODEJS_CATCH_REJECTION=0
    -sEXPORTED_RUNTIME_METHODS=Emval
    #-sASSERTIONS
    -sALLOW_MEMORY_GROWTH
    #-sUSE_CLOSURE_COMPILER=1
    --no-entry

    --post-js=${CMAKE_CURRENT_LIST_DIR}/wrapper.js
)

set_target_properties(spreader.js PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/wrapper.js)

target_sources(spreader.js
PRIVATE
    main.cpp
    wrapper.js
    spreader.d.ts
)

set(EXTRA_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/spreader.d.ts
)

add_custom_target(spreader.js-files 
    DEPENDS ${EXTRA_FILES}
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/spreader.d.ts
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/spreader.d.ts ${CMAKE_CURRENT_BINARY_DIR}/spreader.d.ts
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/spreader.d.ts
)

add_dependencies(spreader.js spreader.js-files)

if (DEFINED SPR_JAVASCRIPT_PACKAGE_DIR)
    install(
    FILES 
        ${EXTRA_FILES} 
        $<TARGET_FILE_DIR:spreader.js>/spreader.js 
        $<TARGET_FILE_DIR:spreader.js>/spreader.wasm 
    DESTINATION 
        ${SPR_JAVASCRIPT_PACKAGE_DIR}/src
    )
endif()


add_subdirectory(test)