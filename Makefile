ifeq ($(OS),Windows_NT)
CFLAGS=$(shell mysql_config --cflags) -DDBUG_OFF -fpermissive
else
CFLAGS=$(shell mysql_config --cflags) -DDBUG_OFF -fPIC -fpermissive
endif

all:
	g++ $(CFLAGS) -Imruby/include -shared -Wall -g mrb_eval.cc -o mrb_eval.so mruby/lib/libmruby.a $(EXTRA_LIB)
