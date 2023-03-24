# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

find_package(PythonExtensions REQUIRED)

#We cannot override OUTPUT_NAME property for this target
#python_extension_module relies on the target name and output name matching
#According to PEP 8 – Style Guide for Python Code
#> When an extension module written in C or C++ has an accompanying Python 
#> module that provides a higher level (e.g. more object oriented) interface, 
#> the C/C++ module has a leading underscore (e.g. _socket).
add_library(_spreader MODULE)

set_target_properties(_spreader PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    C_STANDARD 17
    CXX_STANDARD_REQUIRED True
    C_STANDARD_REQUIRED True
)

target_link_libraries(_spreader spreader)
python_extension_module(_spreader)

if (NOT DEFINED SPR_PYTHON_PACKAGE_DIR)
    set(INSTALL_DESTINATION spreader)
else()
    set(INSTALL_DESTINATION ${SPR_PYTHON_PACKAGE_DIR}/spreader)
endif()

install(TARGETS _spreader LIBRARY DESTINATION ${INSTALL_DESTINATION})

target_compile_definitions(_spreader
PRIVATE
    PY_SSIZE_T_CLEAN
)

target_link_options(_spreader 
PRIVATE
    "$<$<CXX_COMPILER_ID:AppleClang>:-Wl,-object_path_lto,${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lto.o>"
)


target_sources(_spreader
PRIVATE
    src/spreader/common.h
    src/spreader/util.h
    src/spreader/globals.h
    src/spreader/marshalling.h
    src/spreader/arguments.h
    src/spreader/main.cpp
)

# if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

#     add_custom_command(TARGET _spreader POST_BUILD
#         COMMAND dsymutil "$<TARGET_FILE:_spreader>" -o "$<TARGET_FILE:_spreader>.dSYM"
#     )

# endif()

