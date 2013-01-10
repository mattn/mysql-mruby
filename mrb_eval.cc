#ifdef _WIN32
#define _WIN32_WINNT 0x0501 
#include <winsock2.h>
#define _SSIZE_T_
#define _NO_OLDNAMES
typedef PVOID RTL_SRWLOCK;
typedef RTL_SRWLOCK SRWLOCK, *PSRWLOCK;
typedef PVOID CONDITION_VARIABLE, *PCONDITION_VARIABLE;
#endif
#include <my_global.h>

extern "C" {
#include <my_sys.h>
#include <mysql.h>
#include <mruby.h>
#undef INCLUDE_ENCODING
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/variable.h>
}

#ifdef _WIN32
#define EXPORT _declspec(dllexport)
#else
#define EXPORT
#endif

#include <cstring>

extern "C" {
EXPORT my_bool mrb_eval_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
EXPORT void mrb_eval_deinit(UDF_INIT* initid);
EXPORT char* mrb_eval(UDF_INIT* initid, UDF_ARGS* args, char* result, unsigned long* length, char* is_null, char* error);
}

typedef struct {
  mrb_state* mrb;
  struct mrbc_context* ctx;
} mrb_mysql;

EXPORT
my_bool
mrb_eval_init(UDF_INIT* initid, UDF_ARGS* args, char* message) {
  if (args->arg_count != 1) {
    strncpy(message, "mrb_eval: required just one argument", MYSQL_ERRMSG_SIZE);
    return 1;
  }
  if (args->arg_type[0] != STRING_RESULT) {
    strncpy(message, "mrb_eval: argument should be a string", MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->maybe_null[0] = 0;

  mrb_mysql* m = (mrb_mysql*) malloc(sizeof(mrb_mysql));
  m->mrb = mrb_open();
  m->ctx = mrbc_context_new(m->mrb);
  initid->ptr = (char *)(void *) m;
  initid->const_item = 1;

  return 0;
}

EXPORT
void
mrb_eval_deinit(UDF_INIT* initid) {
  mrb_mysql* m = (mrb_mysql*) initid->ptr;
  mrb_state* mrb = m->mrb;
  mrbc_context_free(mrb, m->ctx);
  mrb_close(mrb);
  free(m);
}

void
mrb_init_mrblib(mrb_state *mrb) {
}

void
mrb_init_mrbgems(mrb_state* mrb) {
}

EXPORT
char*
mrb_eval(UDF_INIT* initid, UDF_ARGS* args, char* result, unsigned long* length, char* is_null, char* error) {
  if (args->lengths[0] != 0) {
    mrb_mysql* m = (mrb_mysql*) initid->ptr;
    mrb_state* mrb = m->mrb;

    mrb_value v = mrb_load_string_cxt(mrb, args->args[0], m->ctx);
    v = mrb_funcall(mrb, v, "to_s", 0);
    *length = RSTRING_LEN(v);
    char* p = (char*) malloc(RSTRING_LEN(v) + 1);
    strcpy(p, RSTRING_PTR(v));
    return p;
  }

  *is_null = 1;
  return NULL;
}
