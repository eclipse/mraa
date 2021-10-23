/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <syslog.h>
#include <Python.h>
#include "python/mraapy.h"


// In order to call a python object (all python functions are objects) we
// need to aquire the GIL (Global Interpreter Lock). This may not always be
// necessary but especially if doing IO (like print()) python will segfault
// if we do not hold a lock on the GIL
void
mraa_python_isr(void (*isr)(void*), void* isr_args)
{

    PyGILState_STATE gilstate = PyGILState_Ensure();
    PyObject* arglist;
    PyObject* ret;
    arglist = Py_BuildValue("(O)", isr_args);
    if (arglist == NULL) {
        syslog(LOG_ERR, "gpio: Py_BuildValue NULL");
    } else {
        ret = PyEval_CallObject((PyObject*) isr, arglist);
        if (ret == NULL) {
            syslog(LOG_ERR, "gpio: PyEval_CallObject failed");
            PyObject *pvalue, *ptype, *ptraceback;
            PyObject *pvalue_pystr, *ptype_pystr, *ptraceback_pystr;
            PyObject *pvalue_ustr, *ptype_ustr, *ptraceback_ustr;
            char *pvalue_cstr, *ptype_cstr, *ptraceback_cstr;
            PyErr_Fetch(&pvalue, &ptype, &ptraceback);
            pvalue_pystr = PyObject_Str(pvalue);
            ptype_pystr = PyObject_Str(ptype);
            ptraceback_pystr = PyObject_Str(ptraceback);
            pvalue_ustr = PyUnicode_AsUTF8String(pvalue_pystr);
            pvalue_cstr = PyBytes_AsString(pvalue_ustr);
            ptype_ustr = PyUnicode_AsUTF8String(ptype_pystr);
            ptype_cstr = PyBytes_AsString(ptype_ustr);
            ptraceback_ustr = PyUnicode_AsUTF8String(ptraceback_pystr);
            ptraceback_cstr = PyBytes_AsString(ptraceback_ustr);
            syslog(LOG_ERR, "gpio: the error was %s:%s:%s", pvalue_cstr, ptype_cstr, ptraceback_cstr);
            Py_XDECREF(pvalue);
            Py_XDECREF(ptype);
            Py_XDECREF(ptraceback);
            Py_XDECREF(pvalue_pystr);
            Py_XDECREF(ptype_pystr);
            Py_XDECREF(ptraceback_pystr);
            Py_XDECREF(pvalue_ustr);
            Py_XDECREF(ptype_ustr);
            Py_XDECREF(ptraceback_ustr);
        } else {
            Py_DECREF(ret);
        }
        Py_DECREF(arglist);
    }

    PyGILState_Release(gilstate);
}
