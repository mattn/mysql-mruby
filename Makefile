MRUBY=../mruby
ifeq ($(OS),Windows_NT)
CFLAGS=$(shell mysql_config --cflags) -DDBUG_OFF -fpermissive
else
CFLAGS=$(shell mysql_config --cflags) -DDBUG_OFF -fPIC -fpermissive
endif
LDFLAGS=$(shell mysql_config --libs)

all:
	g++ $(CFLAGS) -I$(MRUBY)/include -shared -Wall -g mrb_eval.cc -o mrb_eval.so $(LDFLAGS) $(MRUBY)/build/host/lib/libmruby.a
