## 目标

提高多种服务端模型的实例源码，测试

现在已经提供 fork，select，poll，epoll 的阻塞非阻塞，同步非同步的模型代码。

能够尽可能的代码复用。

可编译和测试。


## 编译

进入源文件主目录，(需要先安装cmake)执行
```

mkdir build
mkdir bin
cd build
cmake ..
make
make install

```
在bin目录中会生成目标文件

## 测试

```
./object port
nc localhost port

```

