# Build log digest

- Log: `build.log`
- Lines: 26
- Errors: 2
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_ui.cpp(851,2): error: no viable conversion from '(lambda at code/client/cl_ui.cpp:851:2)' to 'int (*)(const char *, char *, int)'
scons: *** [build\release\wolf\code\client\cl_ui.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\client\cl_ui.cpp(N,N): error: no viable conversion from 'file' to 'file' |
| 1 | scons: *** [build\release\wolf\code\client\cl_ui.obj] Error N |

