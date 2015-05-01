# mysql-mruby

MySQL UDF with mruby

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

```
mysql> create table foo(id integer primary key auto_increment, value text);
Query OK, 0 rows affected (0.39 sec)

mysql> insert into foo(value) values('foo');
Query OK, 1 row affected (0.08 sec)

mysql> insert into foo(value) values('boo');
Query OK, 1 row affected (0.02 sec)

mysql> select mrb_eval('ARGV[0]', value) from foo;
+----------------------------+
| mrb_eval('ARGV[0]', value) |
+----------------------------+
| foo                        |
| boo                        |
+----------------------------+
2 rows in set (0.00 sec)
```

## Author

Yasuhiro Matsumoto (a.k.a mattn)
