# Build log digest

- Log: `build.log`
- Lines: 27
- Errors: 3
- Warnings: 1

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_keys.cpp(1676,14): error: ISO C++17 does not allow incrementing expression of type bool [-Wincrement-bool]
code\client\cl_keys.cpp(1680,13): error: cannot decrement expression of type bool
scons: *** [build\release\wolf\code\client\cl_keys.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\client\cl_keys.cpp(N,N): error: ISO C++N does not allow incrementing expression of type bool [-Wincrement-bool] |
| 1 | code\client\cl_keys.cpp(N,N): error: cannot decrement expression of type bool |
| 1 | scons: *** [build\release\wolf\code\client\cl_keys.obj] Error N |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code\client\cl_keys.cpp(N,N): warning: result of comparison of constant N with expression of type 'file' is always false [-Wtautological-constant-compare] |

