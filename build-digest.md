# Build log digest

- Log: `build.log`
- Lines: 47
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_main.cpp(2289,17): error: use of undeclared identifier 'rendererHandle'
code\client\cl_main.cpp(2290,2): error: use of undeclared identifier 'rendererHandle'
scons: *** [build\release\wolf\code\client\cl_main.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | code\client\cl_main.cpp(N,N): error: use of undeclared identifier 'file' |
| 1 | scons: *** [build\release\wolf\code\client\cl_main.obj] Error N |

