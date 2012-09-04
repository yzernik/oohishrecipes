#include <Python.h>

#include "client.h"
#include "clientadd.h"
#include "clientquery.h"

 
static PyObject* py_add_string(PyObject* self, PyObject* args)
{
  char* str;
  int str_id;
  PyArg_ParseTuple(args, "si", &str, &str_id);
  add_string(str, str_id);
  return Py_BuildValue("i", 0);
}

static PyObject* py_query_string(PyObject* self, PyObject* args)
{
  char* str;
  result_list_t *result_list;
  result_node_t *current;
  PyObject* py_result_list;
  PyObject* py_cur_val;
  py_result_list = PyList_New(0);
  PyArg_ParseTuple(args, "s", &str);
  result_list = query_string(str);
  current = result_list->head;
  while(current != NULL)
    {
      py_cur_val = Py_BuildValue("i", current->val); 
      PyList_Append(py_result_list, py_cur_val);
      Py_DECREF(py_cur_val);
      current = current->next;
    }
  return py_result_list;
}
 
static PyMethodDef RecipeSearchMethods[] =
  {
    {"add_string", py_add_string, METH_VARARGS},
    {"query_string", py_query_string, METH_VARARGS},
    {NULL, NULL, 0, NULL}
  };
 
PyMODINIT_FUNC
 
initrecipesearch(void)
{
  (void) Py_InitModule("recipesearch", RecipeSearchMethods);
}
