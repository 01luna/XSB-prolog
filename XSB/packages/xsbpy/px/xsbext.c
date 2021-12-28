#define PY_SSIZE_T_CLEAN

#include <stdio.h>
#include <Python.h>
#include <cinterf.h>
#include "xsbpy_defs.h"
#include <error_xsb.h>
#include <cell_xsb.h>
#include <emuloop.h>
#include <register.h>
#include <emudef.h>
#include <heap_xsb.h>
#include <memory_xsb.h>
#include <flags_xsb.h>

int convert_pyObj_prObj(PyObject *, prolog_term *);
int convert_prObj_pyObj(prolog_term , PyObject **);
void printPlgTerm(prolog_term term2);
void pPO(PyObject *obj1);
static PyObject *px_init();
static PyObject *px_close();
static PyObject *px_query(PyObject *self,PyObject *args);
static PyObject *px_cmd(PyObject *self,PyObject *args);
static PyObject *px_get_error_message();

//void printPyObj(PyObject *self,PyObject *obj1);

//-------------------------------------------------
// Initting
//-------------------------------------------------

static PyMethodDef XsbMethods[] = {
  //    {"printPyObj", printPyObj, METH_VARARGS, "Print Python Obj from C"},
    {"px_init", px_init, METH_VARARGS, "Init XSB"},
    {"px_close", px_close, METH_VARARGS, "Close XSB"},
    {"px_query", px_query, METH_VARARGS, "XSB query execution from Python"},
    {"px_cmd", px_cmd, METH_VARARGS, "XSB command execution from Python"},
    {"px_get_error_message", px_get_error_message, METH_VARARGS, "Find the XSB error message"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyObject *px_get_error_message() {
  return PyUnicode_FromString(xsb_get_error_message(CTXT));
}

static struct PyModuleDef moduledef = {					\
            PyModuleDef_HEAD_INIT, "xsbext", "xsbCextenstion", -1, XsbMethods, \
            NULL,NULL,NULL,NULL };


// To init the Python module
PyMODINIT_FUNC 
PyInit_xsbext(void)
{
    PyObject *module = PyModule_Create(&moduledef);
    if (module == NULL)
      return NULL;
    PyRun_SimpleString(
   "import sys\n"
   "sys.path.append('.')\n");
    
    return module;
}

// TES: need to get from env var
// To init xsb once the Python module has been initted
//static PyObject *pyxsb_init() {
//  char *mychar = "/home/tswift/xsb-repo/xsb-code/XSB";
//  PyObject* ret = PyLong_FromLong((long) xsb_init(1,&mychar)); 
//  return ret;
//}

static PyObject *px_init() {
  //  char *mychar = "/home/tswift/xsb-repo/xsb-code/XSB";
  char *argarray[3];
  argarray[0] =  "/home/tswift/xsb-repo/xsb-code/XSB";
  argarray[1] = "30";
  //  PyObject* ret = PyLong_FromLong((long) xsb_init(1,&mychar)); 
  PyObject* ret = PyLong_FromLong((long) xsb_init(1,argarray)); 
  return ret;
}

static PyObject *px_close() {
  xsb_close();
  return PyLong_FromLong(0);
		
}

#define PYFALSE 0
#define PYTRUE  1
#define PYUNDEF 2

#define get_tv()						 \
  (is_var(reg_term(CTXTc 1))?PyLong_FromLong(PYFALSE):		 \
                             (delayreg?PyLong_FromLong(PYUNDEF): \
                                       PyLong_FromLong(PYTRUE)))

void ensurePyXSBStackSpace(CTXTdeclc PyObject *pyObj) {
  PyObject * thirdArg;
  if (PyTuple_Check(pyObj) && PyTuple_Size(pyObj) > 2) {
    thirdArg = PyTuple_GetItem(pyObj,2);
    if (PyList_Check(thirdArg) || PySet_Check(thirdArg)) {
      printf("list size %ld\n",PyList_Size(thirdArg));
      check_glstack_overflow(5,pcreg,2*PyList_Size(thirdArg)*sizeof(Cell));
    }
    else if (PyDict_Check(thirdArg)) {
      printf("dict size %ld\n",PyDict_Size(thirdArg));
      check_glstack_overflow(5,pcreg,24*PyDict_Size(pyObj)*sizeof(Cell));
    }
    else if (PyTuple_Check(thirdArg)) {
      printf("tuple size %ld\n",PyTuple_Size(thirdArg));
      check_glstack_overflow(5,pcreg,4*PyTuple_Size(pyObj)*sizeof(Cell));
    }
  }
}

PyObject* newPyObj;
//extern CPtr heap_bot;

#define reset_local_heap_ptrs						\
  heap_offset = (CPtr)new_call-(CPtr)gl_bot;				\
  new_call = (prolog_term) ((CPtr)glstack.low + heap_offset) ;		\
  gl_bot = (CPtr)glstack.low;

/* TES: maybe complstack */

#define print_starting_registers			\
  printf("srting hreg %p\n",hreg);			\
  printf("srting hbreg %p\n",hbreg);			\
  printf("srting breg %p\n",breg);			\
  printf("srting ereg %p\n",ereg);			\
  printf("srting ebreg %p\n",ebreg);			\
  printf("srting trreg %p\n",trreg);			\
  printf("srting cpreg %p\n",cpreg);			

#define print_ending_registers			\
  printf("ending hreg %p\n",hreg);		\
  printf("ending hbreg %p\n",hbreg);		\
  printf("ending breg %p\n",breg);		\
  printf("ending ereg %p\n",ereg);		\
  printf("ending ebreg %p\n",ebreg);		\
  printf("ending trreg %p\n",trreg);			\
  printf("ending cpreg %p\n",cpreg);			

#define get_reg_offsets					\
  size_t hreg_offset = hreg - gl_bot;			\
  size_t hbreg_offset = hbreg - gl_bot;			\
  size_t ereg_offset = (CPtr)glstack.high - ereg;	\
  size_t ebreg_offset = (CPtr)glstack.high - ebreg;	\
  size_t trreg_offset = (CPtr) trreg - (CPtr)tcpstack.low;	\
  size_t breg_offset = (CPtr) tcpstack.high - breg;	\
  //  print_starting_registers;

#define reset_regs					\
  hreg = (CPtr)glstack.low + hreg_offset;		\
  hbreg = (CPtr)glstack.low +  hbreg_offset;		\
  ereg = (CPtr)glstack.high - ereg_offset;		\
  ebreg = (CPtr)glstack.high - ebreg_offset;		\
  trreg = (CPtr *)((CPtr)tcpstack.low + trreg_offset);	\
  breg = (CPtr)tcpstack.high - breg_offset;		\
  //     print_ending_registers;  

static PyObject *px_query(PyObject *self,PyObject *args) {
  size_t tuplesize = PyTuple_Size(args);
  size_t heap_offset;
  reset_ccall_error(CTXT);
  ensurePyXSBStackSpace(CTXTc args);
  prolog_term return_pr = p2p_new();
  convert_pyObj_prObj(args, &return_pr);
  //  printPlgTerm(p2p_arg(return_pr, 1));
  //  printf("new prolog obj: ");printPlgTerm(return_pr);
  CPtr gl_bot = (CPtr)glstack.low;
  get_reg_offsets;

  prolog_term new_call = p2p_new();
  c2p_functor_in_mod(p2c_string(p2p_arg(return_pr, 1)),p2c_string(p2p_arg(return_pr, 2)),
		     tuplesize-1,new_call);
  if (gl_bot != (CPtr)glstack.low) printf("2 heap bot old %p new %p\n",gl_bot, glstack.low);
  for (int i = 1; i < (int) tuplesize-1; i++) {
    //    printPlgTerm(p2p_arg(return_pr,3));
    prolog_term call_arg = p2p_arg(new_call,i); 
    p2p_unify(call_arg,p2p_arg(return_pr,i+2));
  }
  if (gl_bot != (CPtr)glstack.low) printf("3 heap bot old %p new %p\n",gl_bot, glstack.low);
  xsb_query_save(tuplesize-1);
if (gl_bot != (CPtr)glstack.low) {
    printf("q4 heap bot old %p new %p\n",gl_bot, glstack.low);
    reset_local_heap_ptrs;
  }
  p2p_unify(reg_term(CTXTc 1),new_call);
  //  printf("about to call: ");printPlgTerm(reg_term(1));
  if (gl_bot != (CPtr)glstack.low) printf("5 heap bot old %p new %p\n",gl_bot, glstack.low);
  c2p_int(CTXTc 0,reg_term(CTXTc 3));  /* set command for calling a goal */
  xsb(CTXTc XSB_EXECUTE,0,0);
  //  printf("before conv: "); printPlgTerm(new_call);
  if (ccall_error_thrown(CTXT))  {
    //    printf("Error: %s\n",xsb_get_error_message(CTXT));
    PyErr_SetString(PyExc_Exception,xsb_get_error_message(CTXT));
    return Py_None;
  } else {
    if (is_var(reg_term(CTXTc 1))) return PyLong_FromLong(PYFALSE);
    else {
      PyObject *tup = PyTuple_New(2);
      if (gl_bot != (CPtr)glstack.low) {
	printf("6 heap bot old %p new %p\n",gl_bot, glstack.low);
	reset_local_heap_ptrs;
      }
      convert_prObj_pyObj(p2p_arg(new_call,tuplesize-1),&newPyObj);
      PyTuple_SET_ITEM(tup,0,newPyObj);
      PyTuple_SET_ITEM(tup,1,(delayreg?PyLong_FromLong(PYUNDEF):PyLong_FromLong(PYTRUE)));
      reset_regs;
      //      pPO(CTXTc tup);
      //      c2p_int(CTXTc 3,reg_term(CTXTc 3));  /* set command for calling a goal */
      //      xsb(CTXTc XSB_EXECUTE,0,0);
      return tup;
    }
  }
}

static PyObject *px_cmd(PyObject *self,PyObject *args) {
  size_t arity;
  size_t tuplesize = PyTuple_Size(args);
  size_t heap_offset;
  arity = tuplesize-2;
  reset_ccall_error(CTXT);
  //  printf("gc margin %ld\n",flags[HEAP_GC_MARGIN]);
  //  ensurePyXSBStackSpace(CTXTc args);
  CPtr gl_bot = (CPtr)glstack.low;
  get_reg_offsets;
  prolog_term return_pr = p2p_new();
  convert_pyObj_prObj(args, &return_pr);
  //  printf("new prolog obj: ");printPlgTerm(return_pr);
  prolog_term new_call = p2p_new();
  c2p_functor_in_mod(p2c_string(p2p_arg(return_pr, 1)),p2c_string(p2p_arg(return_pr, 2)),
		     arity,new_call);
  for (int i = 1; i <= (int) arity; i++) {
    //    printPlgTerm(p2p_arg(return_pr,3));
    prolog_term call_arg = p2p_arg(new_call,i); 
    p2p_unify(call_arg,p2p_arg(return_pr,i+2));
  }
  //  printf("done with first unify\n");
  if (arity==0) xsb_query_save(1);
  else  xsb_query_save(arity);
  if (gl_bot != (CPtr)glstack.low) {
    printf("4 heap bot old %p new %p\n",gl_bot, glstack.low);
    reset_local_heap_ptrs;
  }
  //  printf("done with query_save\n");
  p2p_unify(reg_term(CTXTc 1),new_call);
  //  printf("about to call: ");printPlgTerm(reg_term(1));
  c2p_int(CTXTc 0,reg_term(CTXTc 3));  /* set command for calling a goal */
  xsb(CTXTc XSB_EXECUTE,0,0);
  //  printf("before conv: "); printPlgTerm(new_call);
  if (ccall_error_thrown(CTXT)) {
    PyErr_SetString(PyExc_Exception,xsb_get_error_message(CTXT));
    //    printf("Error: %s\n",xsb_get_error_message(CTXT));
    return Py_None;
    } else {
    PyObject *tv = get_tv();
    reset_regs;
    return tv;
  }
}

//-------------------------------------------------
// Common routines to be factored out.
//-------------------------------------------------

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

int find_prolog_term_type(CTXTdeclc prolog_term term) {
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
    if(strcmp(p2c_functor(term),PYOBJ_C) == 0 ) {
      return PYREF;	
    }
    //    else if(strcmp(p2c_functor(term),"pyList") == 0)
    //      return PPYLIST;
    else if(strcmp(p2c_functor(term),"pyIterator") == 0)
      return PYITER;
    else if(strcmp(p2c_functor(term), PYTUP_C) == 0)
      return PYTUP;
    return PYFUNCTOR;
  }
  else 
    return -1;
}

// -------------------- Python to Prolog
// TES: need to add decrefs

int convert_pyObj_prObj(CTXTdeclc PyObject *pyObj, prolog_term *prTerm) {
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
    if (!result) {
      PyObject *ptype, *pvalue, *ptraceback;
      PyErr_Fetch(&ptype, &pvalue, &ptraceback);
      PyObject* ptypeRepresentation = PyObject_Repr(ptype);
      PyObject* pvalueRepresentation = PyObject_Repr(pvalue);
      PyErr_Restore(ptype, pvalue, ptraceback);
      xsb_abort("++Error[xsbpy]: A Python Error Occurred.  A Python Unicode object "
		"could not be translated to a UTF8 string: %s/%s",
		PyUnicode_AsUTF8(ptypeRepresentation),PyUnicode_AsUTF8(pvalueRepresentation));
    }
    else 
      c2p_string(CTXTc (char *) result, *prTerm);
    return 1;
  }
  else if(pyObj == Py_None){
    char* result = PYNONE_C;
    c2p_string(CTXTc result,*prTerm);
    return 1;
  }
  else if(PyTuple_Check(pyObj))  {
    size_t  i;
    PyObject *pyObjInner = NULL;
    size_t size = PyTuple_Size(pyObj);
    prolog_term P = p2p_new();
    c2p_functor(PYTUP_C,(int)size,P);
    for (i = 0; i < size; i++) {
      pyObjInner = PyTuple_GetItem(pyObj, i);
      prolog_term ithterm = p2p_arg(P, (int)i+1);
      convert_pyObj_prObj(pyObjInner, &ithterm);
    }
    // prolog_term P = convert_pyTuple_prTuple(CTXTc pyObj);
    if(!p2p_unify(CTXTc P, *prTerm))
      return FALSE;
    return TRUE;
  }
  //  else if(flag == 0 && PyList_Check(pyObj)) {
  //    char str[30];
  //    sprintf(str, "p%p", pyObj);
  //    prolog_term ref = p2p_new(CTXT);
  //    c2p_functor(CTXTc "pyList", 1, ref);
  //    prolog_term ref_inner = p2p_arg(ref, 1);
  //    c2p_string(CTXTc str, ref_inner);		
  //    if(!p2p_unify(CTXTc ref, *prTerm))
  //      return FALSE;	
  //    return TRUE;
  //  }
  //  else if(flag == 1 && PyList_Check(pyObj)) {
  else if(PyList_Check(pyObj)) {
    PyObject *pyObjInner;
    size_t size = PyList_GET_SIZE(pyObj); //change tes
    size_t i = 0;
    prolog_term head, tail;
    prolog_term P = p2p_new(CTXT);
    tail = P;
      
    for(i = 0; i < size; i++) {
      c2p_list(CTXTc tail);
      head = p2p_car(tail);
      pyObjInner = PyList_GetItem(pyObj, i);
      convert_pyObj_prObj(CTXTc pyObjInner, &head);	
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
    prolog_term head, tail;
    prolog_term P = p2p_new(CTXT);
    c2p_functor(PYDICT_C,1,P);
    tail = p2p_arg(P, 1);
    
    PyObject *key, *value, *tup;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pyObj, &pos, &key, &value)) {
      //      printf("key: ");printPyObj(CTXTc key);
      //      printf("val: ");printPyObj(CTXTc value);
      c2p_list(CTXTc tail);
      head = p2p_car(tail);
      tup = PyTuple_New(2);
      PyTuple_SET_ITEM(tup,0,key);
      PyTuple_SET_ITEM(tup,1,value);		
      //      printPyObj(CTXTc tup);
      convert_pyObj_prObj(CTXTc tup, &head);	
      tail = p2p_cdr(tail);
    }
    c2p_nil(CTXTc tail);
    if(!p2p_unify(CTXTc P, *prTerm))
      return FALSE;
    return TRUE;
  }
  //  else if(PyAnySet_Check(pyObj)) {           // maybe PyAnySet_Check
  else if(PySet_Check(pyObj)) {           // maybe PyAnySet_Check
    size_t size = PySet_GET_SIZE(pyObj);  // macro version since obj is a set
    size_t i = 0;
    prolog_term head, tail;
    prolog_term P = p2p_new(CTXT);
    PyObject *pyObjInner;

    c2p_functor(PYSET_C,1,P);
    tail = p2p_arg(P, 1);
    
    for(i = 0; i < size; i++) {
      c2p_list(CTXTc tail);
      head = p2p_car(tail);
      pyObjInner = PySet_Pop(pyObj);
      convert_pyObj_prObj(CTXTc pyObjInner, &head);	
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
  else if PyIter_Check(pyObj) {
      printf("found an iterator\n");
    }
  /* default -- not of a type that is handled */
  char str[30];
  sprintf(str, "p%p", pyObj);
  prolog_term ref = p2p_new(CTXT);
  c2p_functor(CTXTc PYOBJ_C, 1, ref);
  prolog_term ref_inner = p2p_arg(ref, 1);
  c2p_string(CTXTc str, ref_inner);		
  if(!p2p_unify(CTXTc ref, *prTerm))
    return FALSE;	
  return TRUE;
}

PyObject *printPyObj(PyObject *self, PyObject *obj1) {
	PyObject* objectsRepresentation = PyObject_Repr(obj1);
	const char* s = PyUnicode_AsUTF8(objectsRepresentation);
	printf("printPyObj: %s\n",s);
	return obj1;
}

void pPO(PyObject *obj1) {
	PyObject* objectsRepresentation = PyObject_Repr(obj1);
	const char* s = PyUnicode_AsUTF8(objectsRepresentation);
	printf("printPyObj: %s\n",s);
}

void printPlgTerm( prolog_term term) {
  XSB_StrDefine(StrArgBuf);
  XSB_StrSet(&StrArgBuf,"");
  print_pterm(CTXTc term,1, &StrArgBuf);
  printf("printPlgTerm: %s\n", StrArgBuf.string);
}

// -------------------- Prolog to Python

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

int convert_prObj_pyObj(CTXTdeclc prolog_term prTerm, PyObject **pyObj) {
  //int type = find_prolog_term_type(CTXTc prTerm);
  //printf("type %i\n",type);
  //printPlgTerm(prTerm);
  if(is_int(CTXTc prTerm)) {
    prolog_term argument = prTerm;
    prolog_int argument_int = p2c_int(argument);
    *pyObj = PyLong_FromSsize_t(argument_int);
    return TRUE;
  }
  else if(is_string(CTXTc prTerm)) {
    prolog_term argument = prTerm;
    char *argument_char = p2c_string(argument);
    *pyObj = PyUnicode_FromString(argument_char);
    return TRUE;
  }
  else if(find_prolog_term_type(CTXTc prTerm) == PYFLOAT) {
    prolog_term argument = prTerm;
    prolog_float argument_float = p2c_float(argument);
    *pyObj = PyFloat_FromDouble(argument_float);
    return TRUE;
  }
  else if(find_prolog_term_type(CTXTc prTerm) == PYLIST || find_prolog_term_type(CTXTc prTerm) == PYNIL ) {
    prolog_term argument = prTerm;
    int count = find_length_prolog_list(argument);
    PyObject *pList = PyList_New(count);
    if(!prlist2pyList(CTXTc argument, pList, count))
      return FALSE;
    *pyObj = pList;
    return TRUE;
  }
  else if (is_functor(CTXTc prTerm)) {
    if(strcmp(p2c_functor(prTerm),PYTUP_C) == 0 ) {
      PyObject *tup, *arg;
      prolog_term temp;
      int arity = p2c_arity(prTerm);
      tup = PyTuple_New(arity);
      for (int i = 1; i <= arity; i++) {
	temp = p2p_arg(prTerm, i);
	convert_prObj_pyObj(CTXTc temp, &arg) ;
	PyTuple_SET_ITEM(tup,(i-1),arg);
      }
      *pyObj = tup;
      return TRUE;
    }
    else if (strcmp(p2c_functor(prTerm),PYSET_C) == 0 ) {
      PyObject *pyset, *pyelt;
      prolog_term list, elt;
      list = p2p_arg(prTerm, 1);
      pyset = PySet_New(NULL);
      while (is_list(list)) {   
	elt = p2p_car(list);
	convert_prObj_pyObj(CTXTc elt, &pyelt);
	PySet_Add(pyset,pyelt); // probably need to check for unhashable errors here
	list = p2p_cdr(list);
      }
      *pyObj = pyset;
      return TRUE;
    }
    else if (strcmp(p2c_functor(prTerm),PYDICT_C) == 0 ) {
      PyObject *pydict, *pykey, *pyval;
      prolog_term list, elt;
      list = p2p_arg(prTerm, 1);
      pydict = PyDict_New();
      while (is_list(list)) {   
	elt = p2p_car(list);
	//	if (!PyTuple_Check(elt) || !(PyTuple_Size(elt) == 2)) return FALSE;
	convert_prObj_pyObj(p2p_arg(elt,1),&pykey);
	convert_prObj_pyObj(p2p_arg(elt,2),&pyval);
	PyDict_SetItem(pydict,pykey,pyval);
	list = p2p_cdr(list);
      }
      *pyObj = pydict;
      return TRUE;
    }
    else if (strcmp(p2c_functor(prTerm),PYOBJ_C) == 0 ) {
      //  else if (find_prolog_term_type(CTXTc prTerm) == PYREF) {
      prolog_term ref = p2p_arg(prTerm, 1);
      char *node_pointer = p2c_string(ref); 
      PyObject *pyobj_ref = (PyObject *)strtoll(node_pointer+1,NULL, 16);
      *pyObj = pyobj_ref;
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

