// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_ERROR_HANDLING_H_INCLUDED
#define SPR_HEADER_ERROR_HANDLING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if (defined(__APPLE__) && defined(__MACH__))
    #include <os/log.h>
#elif defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

#define DO_STRINGIZE(x) #x
#define STRINGIZE(x) DO_STRINGIZE(x)

namespace Spreader {

    [[noreturn]] inline auto fatalError(const char * message) {
        #if (defined(__APPLE__) && defined(__MACH__))
            os_log(OS_LOG_DEFAULT, "fatal error: %s\n", message);
        #elif defined(__EMSCRIPTEN__)
            emscripten_log(EM_LOG_ERROR, "fatal error: %s\n", message);
        #else
            fprintf(stderr, "fatal error: %s\n", message);
        #endif
        abort();
    }
}

#define SPR_FATAL_ERROR(message) fatalError(message ", " __FILE__ "(" STRINGIZE(__LINE__) ")")

//assert that input from outside is correct
#define SPR_ASSERT_INPUT(x) assert(x)
//assert internal logic
#define SPR_ASSERT_LOGIC(x) assert(x)

#endif
