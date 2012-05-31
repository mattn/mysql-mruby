all:
	g++ -Imruby/include -shared -fPIC -Wall -g mrb_eval.cc -o mrb_eval.so mruby/lib/libmruby.a
