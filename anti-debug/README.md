# Mac下ptrace反调试以及反反调试

请不要觉得写代码的都是傻逼，出来混，总是要有点反调试能力的，具体代码参看[ptrac.c](ptrace.c)，总的来说，关键代码就一个，就是在main（或者可以在main之前），调用

```
    ptrace(PT_DENY_ATTACH, 0, NULL, 0);
```

这样就会导致不允许被调试了。

你会发现，这个进程既不允许attach，也不允许直接从lldb里面run。

# 绕过ptrace

解决的办法也很简单，让这个ptrace调用的时候，不传入PT_DENY_ATTACH就可以了。

直接在调用前打一个断点，然后我们可以查头文件得到PT_DENY_ATTACH的值为31（0x1f)，在调用前，将存储该值的寄存器或者内存，修改为0x0a（10,允许attach）即可。


