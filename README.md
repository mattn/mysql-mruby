# mysql-mruby

MyQL UDF with mruby

## Installation

```
$ git clone git://github.com/mattn/mysql-mruby.git
$ cd mysql-mruby
$ make
$ sudo cp mysql_mrb.so /foo/bar/lib/mysql/plugin/
```

```
$ mysql -uroot
mysql> create function mrb_eval returns string soname 'mrb_eval.so';
```

## Usage

```
mysql> SELECT mrb_eval('[1,2,3].map {|x| "hello" + x}');
+-------------------------------------------+
| mrb_eval('[1,2,3].map {|x| "hello#{x}"}') |
+-------------------------------------------+
| ["hello1", "hello2", "hello3"]            |
+-------------------------------------------+
1 row in set (0.00 sec)
```

```
mysql> select mrb_eval('ARGV', now(), now());
+------------------------------------------------+
| mrb_eval('ARGV', now(), now())                 |
+------------------------------------------------+
| ["2015-05-01 19:53:02", "2015-05-01 19:53:02"] |
+------------------------------------------------+
1 row in set (0.00 sec)
```

## Author

Yasuhiro Matsumoto (a.k.a mattn)
