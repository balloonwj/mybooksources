# cgdb 下载与安装

**下载地址**

```
http://cgdb.github.io/
```



下载完 cgdb 之后，进入 cgdb 目录，执行：

```
$ ./configure --prefix=/usr/local
$ make
$ sudo make install
```



**出现错误：**

```
configure: error: CGDB requires curses.h or ncurses/curses.h to build.
```

解决方案：

```
yum install ncurses-devel
```



**出现错误：**

```
configure: error: Please install makeinfo before installing
```

解决方案：

```
yum install texinfo
```

如果是 CentOS 8.0 及以上系统，需要先执行：

```
yum config-manager --set-enabled PowerTools 
```



**出现错误：**

```
configure: error: Please install help2man
```

解决方案：

```
yum install help2man
```



**出现错误：**

```
configure: error: CGDB requires GNU readline 5.1 or greater to link.
  If you used --with-readline instead of using the system readline library,
  make sure to set the correct readline library on the linker search path
  via LD_LIBRARY_PATH or some other facility.
```

解决方案：

```
yum install readline-devel
```



**出现错误：**

```
configure: error: Please install flex before installing
```

解决方案：

```
yum install flex
```

