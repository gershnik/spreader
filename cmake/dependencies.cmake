# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause

include(FetchContent)

set(DECLARED_DEPENDENCIES "")

FetchContent_Declare(sys_string
    GIT_REPOSITORY  https://github.com/gershnik/sys_string.git
    GIT_TAG         v2.12
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
    SOURCE_SUBDIR   lib
)
list(APPEND DECLARED_DEPENDENCIES sys_string)

FetchContent_Declare(isptr
    GIT_REPOSITORY  https://github.com/gershnik/intrusive_shared_ptr.git
    GIT_TAG         v1.4
    GIT_PROGRESS    TRUE
    GIT_SHALLOW     TRUE
)
list(APPEND DECLARED_DEPENDENCIES isptr)

if(NOT SKBUILD)

FetchContent_Declare(catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v3.3.0
    GIT_PROGRESS    TRUE
)

list(APPEND DECLARED_DEPENDENCIES catch2)

endif()

FetchContent_MakeAvailable(${DECLARED_DEPENDENCIES})

get_directory_property(KNOWN_SUBDIRECTORIES SUBDIRECTORIES)
foreach(dir ${KNOWN_SUBDIRECTORIES})
    if (IS_DIRECTORY ${dir})
        foreach(dep ${DECLARED_DEPENDENCIES})
            #check if the subdirectory is "under" the dependency source dir
            string(FIND ${dir} ${${dep}_SOURCE_DIR} match_pos)
            if (match_pos EQUAL 0)
                #and, if so, exclude it from all to prevent installation
                set_property(DIRECTORY ${dir} PROPERTY EXCLUDE_FROM_ALL YES)
                break()
            endif()
        endforeach()
    endif()
endforeach()

set_target_properties(sys_string PROPERTIES FOLDER "Dependencies")
set_target_properties(isptr PROPERTIES FOLDER "Dependencies")
if(TARGET Catch2)
    set_target_properties(Catch2 PROPERTIES FOLDER "Dependencies")
endif()
if(TARGET Catch2WithMain)
    set_target_properties(Catch2WithMain PROPERTIES FOLDER "Dependencies")
endif()
