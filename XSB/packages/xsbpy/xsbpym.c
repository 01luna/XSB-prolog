#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "auxlry.h"
#include "cinterf_defs.h"
#include <basictypes.h>
#include <register.h>
#include <emudef.h>
#include <flags_xsb.h>
#include <heap_xsb.h>
#include <memory_xsb.h>
#include <error_xsb.h>

#include <dlfcn.h>

#include "xsb_config.h"
#ifdef WIN_NT
#define XSB_DLL
//this allows varstring_xsb to compile
#define WINDOWS_IMP
//CFA put this in to override xsb_config_aux.h's incorrect value of 8
//#define SIZEOF_LONG 4
#endif

//author: Muthukumar Suresh muthukumar5393@gmail.com
#include <cinterf.h>
#include <context.h>
#include <inttypes.h>

//#include <stdio.h>
#ifndef WIN_NT
#include <stdlib.h>
#endif
//#include <string.h>
//region : function decleration(s)
int convert_pyObj_prObj(CTXTdeclc PyObject *, prolog_term *, int);
int convert_prObj_pyObj(CTXTdeclc prolog_term , PyObject **);
void printPlgTerm(prolog_term term2);
void printPyObj(PyObject *obj1);
void printPyObjType(CTXTdeclc PyObject *obj1);
//endregion
//#include "xsbpmodule.c"

enum prolog_term_type 
{
	PYINT = 0,
	PYFLOAT = 1,
	PYSTRING = 2,
	PYLIST = 3,
	PYNIL = 4,
	PYFUNCTOR = 5,
	PYVAR = 6,
	PYREF = 7,
	PYITER = 8,
	PYTUP = 9,
	PPYLIST = 10, 
};

//CFA - portable replacement for setenv
// TES -- maybe works for windows, but use stdlib for unix.
#ifdef WIN_NT
int setenv(const char *name, const char *value, int overwrite)
{
    int errcode = 0;
    if(!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if(errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}
#endif
int is_reference(prolog_term term)
{
	char *result = p2c_string(term);
	if(strncmp("ref_", result, 4)== 0 ){
		return 1;
	}
	return 0;
}

int find_prolog_term_type(CTXTdeclc prolog_term term)
{
	if(is_float(term))
		return PYFLOAT;
	else if(is_int(term))
		return PYINT;
	else if(is_list(term))
		return PYLIST;
	else if(is_var(term))
		return PYVAR;
	else if(is_nil(term))
		return PYNIL;
  else if(is_string(term))
    return PYSTRING;
	else if(is_functor(term))	{
		if(strcmp(p2c_functor(term),"pyObject") == 0 ) {
			return PYREF;	
		}
		else if(strcmp(p2c_functor(term),"pyList") == 0)
			return PPYLIST;
		else if(strcmp(p2c_functor(term),"pyIterator") == 0)
			return PYITER;
		else if(strcmp(p2c_functor(term), "pyTuple") == 0)
			return PYTUP;
		return PYFUNCTOR;
	}
	else 
		return -1;
}

int find_length_prolog_list(prolog_term V)
{
	prolog_term temp = V;
	int count= 0;
	while(!(is_nil(temp)))
	{
		p2p_car(temp);
		count++;
		temp = p2p_cdr(temp);
	}
	return count;
}

void ensureXSBStackSpace(CTXTdeclc PyObject *pyObj) {
  if (PyList_Check(pyObj)) {
    check_glstack_overflow(3,pcreg,2*PyList_Size(pyObj)*sizeof(size_t));
  }
  if (PyTuple_Check(pyObj)) {
    check_glstack_overflow(3,pcreg,2*PyTuple_Size(pyObj)*sizeof(size_t));
  }
}

int prlist2pyList(CTXTdeclc prolog_term V, PyObject *pList, int count)
{
	prolog_term temp = V;
	prolog_term head;
	int i;
	for(i = 0; i <count;i++)
	{ 
		head = p2p_car(temp);
		PyObject *pyObj = NULL;
		if( !convert_prObj_pyObj(CTXTc head, &pyObj))
			return FALSE;
		PyList_SetItem(pList, i, pyObj);
		temp = p2p_cdr(temp);
	}	
	return TRUE;
}

prolog_term convert_pyTuple_prTuple(CTXTdeclc PyObject *pyObj){
  size_t size = PyTuple_Size(pyObj);
	
  prolog_term P = p2p_new(CTXT);
  if(size >=2){
    //		c2p_functor(CTXTc ",", 2, P);
    c2p_functor(CTXTc ",", 2, P);
    prolog_term temp = P;
    int i = 1;
    while(i <size-1) {
      PyObject *pyObjinner = PyTuple_GetItem(pyObj, i-1);
      prolog_term ithterm = p2p_arg(temp, 1);
      convert_pyObj_prObj(CTXTc pyObjinner, &ithterm, 0);
      temp = p2p_arg(temp,2);
      c2p_functor(CTXTc ",", 2, temp);
      i++;
    }
    PyObject *pyObjinner = PyTuple_GetItem(pyObj, i-1);
    prolog_term ithterm = p2p_arg(temp, 1);
    convert_pyObj_prObj(CTXTc pyObjinner, &ithterm, 0);
    ithterm = p2p_arg(temp, 2);
    pyObjinner = PyTuple_GetItem(pyObj, i);
    convert_pyObj_prObj(CTXTc pyObjinner, &ithterm, 0);
  }
  else
    {
      c2p_functor(CTXTc ",", size, P);
      if(size == 1){
	PyObject *pyObjinner = PyTuple_GetItem(pyObj, 0);
	prolog_term ithterm = p2p_arg(P, 1);
	convert_pyObj_prObj(CTXTc pyObjinner, &ithterm, 0);
      }
    }
  return P;
}

int convert_pyObj_prObj(CTXTdeclc PyObject *pyObj, prolog_term *prTerm, int flag) {
  if(pyObj == Py_None){
    return 1;// todo: check this case for a list with a none in the list. how does prolog side react 
  }
  if(PyLong_Check(pyObj)) {
    prolog_int result = PyLong_AsSsize_t(pyObj);
    c2p_int(CTXTc result, *prTerm);
    return 1;
  }
  else if(PyFloat_Check(pyObj)) {
    double result = PyFloat_AS_DOUBLE(pyObj);
    c2p_float(CTXTc result, *prTerm);
    return 1;
  }
  else if(PyUnicode_Check(pyObj)) {
    const  char *result = PyUnicode_AsUTF8(pyObj);		
    c2p_string(CTXTc (char *) result, *prTerm);
    return 1;
  }
  else if(PyTuple_Check(pyObj))  {
    size_t  i;
    size_t size = PyTuple_Size(pyObj);
    prolog_term P = p2p_new();
    c2p_functor("",size,P);
    for (i = 0; i < size; i++) {
      PyObject *pyObjinner = PyTuple_GetItem(pyObj, i);
      prolog_term ithterm = p2p_arg(P, i+1);
      convert_pyObj_prObj(pyObjinner, &ithterm,1);
    }
    // prolog_term P = convert_pyTuple_prTuple(CTXTc pyObj);
    if(!p2p_unify(CTXTc P, *prTerm))
      return FALSE;
    return TRUE;
  }
  else if(flag == 0 && PyList_Check(pyObj)) {
    char str[30];
    sprintf(str, "p%p", pyObj);
    prolog_term ref = p2p_new(CTXT);
    c2p_functor(CTXTc "pyList", 1, ref);
    prolog_term ref_inner = p2p_arg(ref, 1);
    c2p_string(CTXTc str, ref_inner);		
    if(!p2p_unify(CTXTc ref, *prTerm))
      return FALSE;	
    return TRUE;
  }
  else if(flag == 1 && PyList_Check(pyObj)) {
    size_t size = PyList_Size(pyObj);
    size_t i = 0;
    prolog_term head, tail;
    prolog_term P = p2p_new(CTXT);
    tail = P;
      
    for(i = 0; i < size; i++) {
      c2p_list(CTXTc tail);
      head = p2p_car(tail);
      PyObject *pyObjInner = PyList_GetItem(pyObj, i);
      convert_pyObj_prObj(CTXTc pyObjInner, &head, 1);	
      //printPyObj(CTXTc pyObjInner);
      //printPyObjType(CTXTc pyObjInner);
      //printPlgTerm(CTXTc head);
      tail = p2p_cdr(tail);
    }
    c2p_nil(CTXTc tail);
    if(!p2p_unify(CTXTc P, *prTerm))
      return FALSE;
    return TRUE;
  }
  else if(PyDict_Check(pyObj)) {
    //    printf("found pydict obj\n");
    prolog_term head, tail;
    prolog_term P = p2p_new(CTXT);
    tail = P;
    
    PyObject *key, *value, *tup;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pyObj, &pos, &key, &value)) {
      //      printf("key: ");printPyObj(CTXTc key);
      //      printf("val: ");printPyObj(CTXTc value);
      c2p_list(CTXTc tail);
      head = p2p_car(tail);
      tup = PyTuple_New(3);
      PyTuple_SET_ITEM(tup,0,key);
      PyTuple_SET_ITEM(tup,1,PyUnicode_FromString(":"));
      PyTuple_SET_ITEM(tup,2,value);		
      //      printPyObj(CTXTc tup);
      convert_pyObj_prObj(CTXTc tup, &head, 1);	
      tail = p2p_cdr(tail);
    }
    c2p_nil(CTXTc tail);
    if(!p2p_unify(CTXTc P, *prTerm))
      return FALSE;
    return TRUE;
  }
  /* default -- not of a type that is handled */
  char str[30];
  sprintf(str, "p%p", pyObj);
  prolog_term ref = p2p_new(CTXT);
  c2p_functor(CTXTc "pyObject", 1, ref);
  prolog_term ref_inner = p2p_arg(ref, 1);
  c2p_string(CTXTc str, ref_inner);		
  if(!p2p_unify(CTXTc ref, *prTerm))
    return FALSE;	
  return TRUE;
}

int convert_prObj_pyObj(CTXTdeclc prolog_term prTerm, PyObject **pyObj) {
	//int type = find_prolog_term_type(CTXTc prTerm);
	//printf("type %i\n",type);
	//printPlgTerm(prTerm);
	if(find_prolog_term_type(CTXTc prTerm) == PYINT)
	{
		prolog_term argument = prTerm;
		prolog_int argument_int = p2c_int(argument);
		*pyObj = PyLong_FromSsize_t(argument_int);
		return TRUE;
	}
	else if(find_prolog_term_type(CTXTc prTerm) == PYSTRING)
	{
		prolog_term argument = prTerm;
		char *argument_char = p2c_string(argument);
		*pyObj = PyUnicode_FromString(argument_char);
		return TRUE;
	}
	else if(find_prolog_term_type(CTXTc prTerm) == PYFLOAT)
	{
		prolog_term argument = prTerm;
		prolog_float argument_float = p2c_float(argument);
		*pyObj = PyFloat_FromDouble(argument_float);
		return TRUE;
	}
	else if(find_prolog_term_type(CTXTc prTerm) == PYLIST || find_prolog_term_type(CTXTc prTerm) == PYNIL )
	{
		prolog_term argument = prTerm;
			int count = find_length_prolog_list(argument);
			PyObject *pList = PyList_New(count);
			if(!prlist2pyList(CTXTc argument, pList, count))
				return FALSE;
			*pyObj = pList;
			return TRUE;
	}
	
	else if (find_prolog_term_type(CTXTc prTerm) == PYREF)
	{
			prolog_term ref = p2p_arg(prTerm, 1);
			char *node_pointer = p2c_string(ref); 
			PyObject *pyobj_ref = (PyObject *)strtoll(node_pointer+1,NULL, 16);
			*pyObj = pyobj_ref;
			return TRUE;
	}
	return FALSE;
}

int set_python_argument(CTXTdeclc prolog_term temp, PyObject *pArgs,int i)
{
	PyObject *pValue;
	if(!convert_prObj_pyObj(CTXTc temp, &pValue))
		return FALSE;
	PyTuple_SetItem(pArgs, i-1, pValue);
	return TRUE;
}

//////

void build_result(char *res, PyObject **lis)
{
	char * pch;
	pch = strtok (res, "|");
	//	int counter = 1;
	while(pch!= NULL){
    //		PyList_Append(*lis, PyString_FromString(pch));
    PyList_Append(*lis, PyUnicode_FromString(pch));
		pch = strtok (NULL, "|");
		
	}
}
static PyObject *xsbp_querySingle(PyObject *self, PyObject *args)
{
	char *cmd;
	if(!PyArg_ParseTuple(args, "s", &cmd))
		return NULL;
	int rcp;
	XSB_StrDefine(p_return_string);
	PyObject* resultList = PyList_New(0);
	xsb_query_save(3);
	rcp = xsb_query_string_string(cmd,&p_return_string,"|");
	xsb_close_query();
	xsb_query_restore();
	PyObject *lis = PyList_New(0);
	if(rcp == XSB_SUCCESS){
	build_result(p_return_string.string, & lis);
	PyList_Append(resultList,lis);
	}
	return resultList;
}

static PyObject * xsbp_queryAll(PyObject *self, PyObject *args)
{
	char *cmd;
	if(!PyArg_ParseTuple(args, "s", &cmd))
		return NULL;
	printf("%s", cmd);
	int rcp;
	XSB_StrDefine(p_return_string);
	
	xsb_query_save(3);
	rcp = xsb_query_string_string(cmd,&p_return_string,"|");
	printf("here2");
	while (rcp == XSB_SUCCESS ) {
	 	printf("Return p %s\n",(p_return_string.string));
	 	rcp = xsb_next_string(&p_return_string,"|");
	 }
	xsb_query_restore();
	return Py_BuildValue("s", p_return_string.string);
}

char *set_path_name(char *module)
{
  char *directory_end = strrchr(module, '/');
	if(directory_end == NULL)
		return module;
	size_t directory_len = (size_t)(directory_end - module);//no need for last '/'
	// char *directory = malloc(strlen(getenv("PYTHONPATH")) + directory_len+2);
	// memset(directory, '\0',strlen(getenv("PYTHONPATH")) + directory_len+2);
	// strncpy(directory,getenv("PYTHONPATH"), strlen(getenv("PYTHONPATH")));
	// strncpy(directory+strlen(getenv("PYTHONPATH")), ":", 1);
	// strncpy(directory+strlen(getenv("PYTHONPATH"))+1,module, directory_len);
	// setenv("PYTHONPATH", directory,1);
	char *directory = malloc(directory_len+1);
	memset(directory, '\0',directory_len+1);
	strncpy(directory,module, directory_len);
	PyObject* sysPath = PySys_GetObject((char*)"path");
	PyObject* programName = PyUnicode_FromString(directory);
	PyList_Append(sysPath, programName);
	Py_DECREF(programName);
	free(directory);
	module = (module + directory_len + 1);
	return module;
}

static PyMethodDef XsbMethods[] = {
    {"querySingle",  xsbp_querySingle, METH_VARARGS,
     "Query XSB from Python which returns the first response."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, "xsbpym", "xsb to python", -1, XsbMethods, \
            NULL,NULL,NULL,NULL };

struct module_state {
    PyObject *error;
};

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

PyMODINIT_FUNC 
PyInit_xsbpym(void)
{
    PyObject *module = PyModule_Create(&moduledef);
    if (module == NULL)
      return NULL;
    //struct module_state *st = GETSTATE(module);

    //st->error = PyErr_NewException("myextension.Error", NULL, NULL);
    //if (st->error == NULL) {
    //  Py_DECREF(module);
    //  return NULL;
    //}
    
    PyRun_SimpleString(
   "import sys\n"
   "sys.path.append('.')\n");
    
    return module;
    //Py_InitModule("xsbp", XsbMethods);
}

//todo: need to refactor this code.
DllExport int callpy(CTXTdecl) {
  prolog_term mod = extern_reg_term(1);
  if(!Py_IsInitialized()) {
    const char *pylib = getenv( "PYTHON_LIBRARY" );
    if (pylib) {
      printf("pylib is: %s\n",pylib);
      dlopen( pylib, RTLD_LAZY | RTLD_GLOBAL );
    } else {
      //      printf("no PYTHON_LIBRARY FOUND\n");
      dlopen("/usr/lib/x86_64-linux-gnu/libpython3.7m.so.1.0", RTLD_LAZY | RTLD_GLOBAL );
    }
    Py_Initialize();
  }
  char *module = p2c_string(mod);
  module = set_path_name(module);
  PyObject *pName = NULL, *pModule = NULL, *pFunc = NULL;
  PyObject *pArgs = NULL, *pValue = NULL;
  prolog_term V, temp;
  PyErr_Clear();
  //initasynccall();
  PyInit_xsbpym();
  pName = PyUnicode_FromString(module);
  pModule = PyImport_ImportModule(module);
  if(pModule == NULL) {
    PyErr_Print();
    xsb_abort("++Error[Python/XSB]: no Python module named \'%s\' could be found."
	      "(in arg 1 of callpy/3)\n",module);
  }
  Py_DECREF(pName);
  V = extern_reg_term(2);
  char *function = p2c_functor(V);
  if(is_functor(V)) {
    int args_count = p2c_arity(V);
    pFunc = PyObject_GetAttrString(pModule, function);
    Py_DECREF(pModule);
    if(pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(args_count);
      int i;
      for(i = 1; i <= args_count; i++) {
	temp = p2p_arg(V, i);
	if(!(set_python_argument(CTXTc temp, pArgs, i))) {
	  printf("false set_arg %i\n",i);			
	  return FALSE;
	}
      }
    }
    else {
      xsb_abort("++Error[Python/XSB]: \'%s\' is not a callable function in "
		"the Python module \'%s\' (in arg 1 of callpy/3)\n",function,module);
    }
    //printPyObj(CTXTc pFunc);
    //printPyObj(CTXTc pArgs);

    pValue = PyObject_CallObject(pFunc, pArgs);
    //    printPyObj(CTXTc pValue);
    if (pValue == NULL) { // TES todo change to check for python error
      //      PyErr_Print();  
      //      PyObject *ptype, *pvalue, *ptraceback;
      //PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      //pvalue contains error message
      //ptraceback contains stack snapshot and many other information
      //(see python traceback structure)
      //      xsb_abort("++Error[Python/XSB]: A Python Error Occurred: %s",PyStr_AsString(pvalue));
      xsb_abort("++Error[Python/XSB]: A Python Error Occurred");
    }
    // TES took out next 2 unused vars
    //		PyTypeObject* type = pValue->ob_type;
    //		const char* ptype = type->tp_name;
    //printPyObjType(CTXTc type);
    prolog_term return_pr = p2p_new(CTXT);
    ensureXSBStackSpace(CTXTc pValue);
    if(!convert_pyObj_prObj(CTXTc pValue, &return_pr, 1))
      //		if(!convert_pyObj_prObj(CTXTc pValue, &return_pr, 0))
      return FALSE;
    // TES
    //		prolog_term prTerm = reg_term(CTXTc 3);
    //printPyObj(CTXTc pValue);
    //printPyObjType(CTXTc type);
    //printPlgTerm(CTXTc return_pr);
    if(!p2p_unify(CTXTc return_pr, reg_term(CTXTc 3)))
      return FALSE;
    return TRUE;
  }
  else	{
    return FALSE;
  }
  Py_Finalize();
  return TRUE;
}

DllExport int pyObj_GetIter(CTXTdecl)
{
	prolog_term prTerm = extern_reg_term(1);
	if(find_prolog_term_type(CTXTc prTerm) == PYREF 
		|| find_prolog_term_type(CTXTc prTerm) == PYTUP 
		|| find_prolog_term_type(CTXTc prTerm) == PPYLIST)
	{
	  //		int temp = find_prolog_term_type(CTXTc prTerm);
		prolog_term ref_pyobj = p2p_arg(prTerm, 1);
		char *node_pointer = p2c_string(ref_pyobj); 
		//		long long temp2 = strtoll(node_pointer+1,NULL, 16);
		PyObject *pValue = (PyObject *)strtoll(node_pointer+1,NULL, 16);
		PyObject *iterator; 
		iterator = PyObject_GetIter(pValue);
		if(iterator == NULL)
			return FALSE;
		char str[30];
		sprintf(str, "p%p", iterator);
	  	prolog_term ref = p2p_new(CTXT);
		c2p_functor(CTXTc "pyIterator", 1, ref);
		prolog_term ref_inner = p2p_arg(ref, 1);
    	c2p_string(CTXTc str, ref_inner);		
		if(!p2p_unify(CTXTc ref, reg_term(CTXTc 2)))
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

DllExport int pyObj_Next(CTXTdecl)
{
	prolog_term prTerm = extern_reg_term(1);
	if(find_prolog_term_type(CTXTc prTerm) == PYITER)
	{
		prolog_term ref_pyobj = p2p_arg(prTerm, 1);
		char *node_pointer = p2c_string(ref_pyobj); 
		PyObject *iterator = (PyObject *)strtoll(node_pointer+1,NULL, 16);
		PyObject *obj = PyIter_Next(iterator);
		if(obj == NULL)
			return FALSE;
		prolog_term return_pr = p2p_new(CTXT);
		if(!convert_pyObj_prObj(CTXTc obj, &return_pr, 0))
			return FALSE;
		prolog_term prTerm = extern_reg_term(2);
		if(!p2p_unify(CTXTc return_pr, prTerm))
			return FALSE;
		return TRUE;
	} 
	return FALSE;
}

DllExport int convertPyPr(CTXTdecl)
{
	pyObj_GetIter();
	return pyObj_Next();
}

DllExport int pyTuple2prTuple(CTXTdecl) {
	prolog_term prTerm = extern_reg_term(1);
	if(find_prolog_term_type(CTXTc prTerm) == PYTUP) {
	  prolog_term ref = p2p_arg(prTerm, 1);
	  char *node_pointer = NULL;
	  node_pointer = p2c_string(ref);
	  PyObject *pyobj_ref = (PyObject *)strtoll(node_pointer+1,NULL, 16);
	  if ( pyobj_ref == NULL)
	    return FALSE;
	  printf("size %ld sizeof %ld\n",PyTuple_Size(pyobj_ref),sizeof(size_t));
	  PyObject * pyObj = pyobj_ref;
	  if (pyObj == NULL)
	    return FALSE;
	  check_glstack_overflow(3,pcreg,2*PyTuple_Size(pyobj_ref)*sizeof(size_t));
	  if(PyTuple_Check(pyObj))
	    {
	      size_t size = PyTuple_Size(pyObj);
	      size_t i = 0;
	      //	      prolog_term head, tail;
	      prolog_term P = p2p_new(CTXT);
	      c2p_functor("",size,P);
	      for (i = 0; i < size; i++)
		{
		  PyObject *pyObjinner = PyTuple_GetItem(pyObj, i);
		  prolog_term ithterm = p2p_arg(P, i+1);
		  convert_pyObj_prObj(pyObjinner, &ithterm,1);
		}
	      //	      tail = P;
	      //for(i = 0; i < size; i++) {
	      //c2p_list(CTXTc tail);
	      //	head = p2p_car(tail);
	      //PyObject *pyObjInner = PyTuple_GetItem(pyObj, i);
	      //convert_pyObj_prObj(CTXTc pyObjInner, &head, 1);
		//printPyObj(CTXTc pyObjInner);
		//printPyObjType(CTXTc pyObjInner);
		//printPlgTerm(CTXTc head);
	      //tail = p2p_cdr(tail);
	      //}
	      //	      c2p_nil(CTXTc tail);
	      prolog_term prTerm = extern_reg_term(2);
	      if(!p2p_unify(CTXTc P, prTerm))
		return FALSE;
	      return TRUE;
	    }		
	}
	return FALSE;
}

DllExport int pyList2prList(CTXTdecl)
{
	prolog_term prTerm = extern_reg_term(1);
	if(find_prolog_term_type(CTXTc prTerm) == PPYLIST)
	{ 
		prolog_term ref = p2p_arg(prTerm, 1);
		char *node_pointer = NULL;
		node_pointer = p2c_string(ref);
		PyObject *pyobj_ref = (PyObject *)strtoll(node_pointer+1,NULL, 16);
		if ( pyobj_ref == NULL)
			return FALSE;
		printf("size %ld sizeof %ld\n",PyList_Size(pyobj_ref),sizeof(size_t));
		check_glstack_overflow(3,pcreg,2*PyList_Size(pyobj_ref)*sizeof(size_t));
		PyObject * pyObj = pyobj_ref;
		if (pyObj == NULL)
			return FALSE;
		if(PyList_Check(pyObj))
		{
			size_t size = PyList_Size(pyObj);
			size_t i = 0;
			prolog_term head, tail;
			prolog_term P = p2p_new(CTXT);
			tail = P;
		
			for(i = 0; i < size; i++)
			{
				c2p_list(CTXTc tail);
				head = p2p_car(tail);
				PyObject *pyObjInner = PyList_GetItem(pyObj, i);
				convert_pyObj_prObj(CTXTc pyObjInner, &head, 1);
				//printPyObj(CTXTc pyObjInner);
				//printPyObjType(CTXTc pyObjInner);
				//printPlgTerm(CTXTc head);

				tail = p2p_cdr(tail);
			}
			c2p_nil(CTXTc tail);
			prolog_term prTerm = extern_reg_term(2);
			if(!p2p_unify(CTXTc P, prTerm))
				return FALSE;
			return TRUE;
		}		
	}
	return FALSE;
}

void printPlgTerm(CTXTdeclc prolog_term term) {
	XSB_StrDefine(StrArgBuf);
  XSB_StrSet(&StrArgBuf,"");
  print_pterm(CTXTc term,1, &StrArgBuf);
  printf("printPlgTerm: %s\n", StrArgBuf.string);
}

void printPyObj(CTXTdeclc PyObject *obj1) {
	PyObject* objectsRepresentation = PyObject_Repr(obj1);
	const char* s = PyUnicode_AsUTF8(objectsRepresentation);
	printf("printPyObj: %s\n",s);
}

void printPyObjType(CTXTdeclc PyObject *obj1) {
	PyTypeObject* type = obj1->ob_type;
	const char* ptype = type->tp_name;
	printf("python type: %s\n",ptype);
}

