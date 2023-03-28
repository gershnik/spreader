# Copyright (c) 2022, Eugene Gershnik
# SPDX-License-Identifier: BSD-3-Clause


if (NOT DEFINED SPR_JAVASCRIPT_PACKAGE_DIR)
    file(REAL_PATH ${CMAKE_CURRENT_BINARY_DIR}/.. SPR_JAVASCRIPT_PACKAGE_DIR)    
endif()

#SPR_JAVASCRIPT_PACKAGE_DIR affects the configuration
configure_file(package.json.config ${CMAKE_CURRENT_LIST_DIR}/package.json @ONLY)

add_custom_target(run-spreader.js-test ALL
    COMMAND npm install 
    COMMAND npm run test
    DEPENDS spreader.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}

    SOURCES 
        cell-access.js
        copy-move.js
        insert-delete.js
        length-info.js
        lifecycle.js
        parsing.js
        recalc.js
        util.js

        package.json.config
)