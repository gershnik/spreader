# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.24)

# Honor visibility properties for all target types.
set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
# INTERPROCEDURAL_OPTIMIZATION is enforced when enabled
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# option() honors normal variables
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_OBJCXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set(BUILD_SHARED_LIBS OFF)

if (SKBUILD)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

#if(DEFINED EMSCRIPTEN)
#    set(CMAKE_EXECUTABLE_SUFFIX_CXX ".html")
#endif()

if(SKBUILD OR DEFINED EMSCRIPTEN)
    set(SPR_SINGLE_THREADED_DEFAULT ON)
else()
    set(SPR_SINGLE_THREADED_DEFAULT OFF)
endif()

option(SPR_SINGLE_THREADED "whether to assume single threaded acess" ${SPR_SINGLE_THREADED_DEFAULT})

project(spreader)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
endif()

include(cmake/dependencies.cmake)

add_subdirectory(lib/code library)

if(SKBUILD) 
    
    add_subdirectory(wrappers/python)

else()
    
    add_subdirectory(lib/test test)

    add_custom_target(test DEPENDS run-library-test)

    if(DEFINED EMSCRIPTEN)

        add_subdirectory(wrappers/javascript)

        add_dependencies(test run-spreader.js-test)

    endif()

endif()




