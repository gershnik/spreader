// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

using namespace std;

int
main(int argc, char *argv[]) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);  /* optional but recommended */
    Py_Initialize();

    string cmd = "import pytest\n"
                 "pytest.main([";
    
    for(int i = 1; i < argc; ++i) {
        cmd += "'";
        cmd += argv[i];
        cmd += "',";
    }
    cmd += "])\n";

    PyRun_SimpleString(cmd.c_str());
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
    return 0;
}

