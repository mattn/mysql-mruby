#include <my_global.h>

extern "C" {
#include <my_sys.h>
#include <mysql.h>
#include <mruby.h>
#undef INCLUDE_ENCODING
#include <mruby/compile.h>
#include <mruby/proc.h>
#include <mruby/string.h>
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

  initid->ptr = (char *)(void *) mrb_open();
  initid->const_item = 1;

  return 0;
}

EXPORT
void
mrb_eval_deinit(UDF_INIT* initid) {
  mrb_close((mrb_state*) initid->ptr);
}

EXPORT
char*
mrb_eval(UDF_INIT* initid, UDF_ARGS* args, char* result, unsigned long* length, char* is_null, char* error) {
  mrb_state *mrb;
  struct mrb_parser_state* st;
  int n;
  char* p;
  mrb_value v;
  if (args->lengths[0] == 0)
    goto error;

  mrb = (mrb_state*) initid->ptr;
  st = mrb_parse_string(mrb, args->args[0]);
  n = mrb_generate_code(mrb, st->tree);
  mrb_pool_close(st->pool);
  v = mrb_run(mrb, mrb_proc_new(mrb, mrb->irep[n]), mrb_nil_value());
  v = mrb_funcall(mrb, v, "to_s", 0);
  *length = RSTRING_LEN(v);
  p = (char*) malloc(RSTRING_LEN(v) + 1);
  strcpy(p, RSTRING_PTR(v));
  return p;

error:
  *is_null = 1;
  return NULL;
}
