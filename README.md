# patcher

A wired implementation for mocking a C function at runtime. Note that the mock function needs to maintain the same signature as the original function. Currently supports Linux on amd64 architecture.

一个奇怪的实现，可以运行时mock掉某个C函数。注意mock函数需要和原函数形式保持一致。目前支持linux amd64

## how to use
```
./build.sh
./test
```