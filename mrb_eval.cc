#ifdef _WIN32
#define _WIN32_WINNT 0x0501 
#include <winsock2.h>
#define _SSIZE_T_
#define _NO_OLDNAMES
#ifdef __MINGW64_VERSION_MAJOR
typedef PVOID RTL_SRWLOCK;
typedef RTL_SRWLOCK SRWLOCK, *PSRWLOCK;
typedef PVOID CONDITION_VARIABLE, *PCONDITION_VARIABLE;
#endif
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
#include <mruby/array.h>
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
  if (args->arg_count == 0) {
    strncpy(message, "mrb_eval: required just one argument at least", MYSQL_ERRMSG_SIZE);
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
  if (args->arg_count > 0) {
    mrb_mysql* m = (mrb_mysql*) initid->ptr;
    mrb_state* mrb = m->mrb;
    char *p;
    int n;

    mrb_value argv = mrb_ary_new(mrb);
    for (n = 1; n < args->arg_count; n++) {
      switch (args->arg_type[n]) {
      case STRING_RESULT:
        mrb_ary_push(mrb, argv, mrb_str_new_cstr(mrb, args->args[n]));
        break;
      case INT_RESULT:
        mrb_ary_push(mrb, argv, mrb_fixnum_value((mrb_int)*(int*)(args->args[n])));
        break;
      case REAL_RESULT:
        mrb_ary_push(mrb, argv, mrb_float_value(mrb, (mrb_float)*(double*)&(args->args[n])));
        break;
      default:
        *error = 1;
        return strdup("invalid type");
      }
    }
    mrb_define_global_const(mrb, "ARGV", argv);

    mrb_value v = mrb_load_string_cxt(mrb, args->args[0], m->ctx);
    if (mrb->exc == 0) {
      v = mrb_funcall(mrb, v, "to_s", 0);
      if (mrb->exc == 0) {
        *length = RSTRING_LEN(v);
        p = (char*) malloc(RSTRING_LEN(v) + 1);
        if (p) {
          strcpy(p, RSTRING_PTR(v));
          return p;
        }
        *is_null = 1;
        return NULL;
      }
    }
    mrb_value exc = mrb_obj_value(mrb->exc);
    mrb_value inspect = mrb_inspect(mrb, exc);
    *error = 1;
    p = mrb_str_to_cstr(mrb, inspect);
    *length = strlen(p);
    return strdup(p);
  }

  *is_null = 1;
  return NULL;
}
