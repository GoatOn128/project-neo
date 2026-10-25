# Build log digest

- Log: `build.log`
- Lines: 25
- Errors: 4
- Warnings: 1

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_cin.cpp(1350,35): error: cannot decrement expression of type bool
code\client\cl_cgame.cpp(521,2): error: cannot initialize a member subobject of type 'bool (*)(int, const char *)' with an lvalue of type 'int (int, const char *)': different return type ('bool' vs 'int')
scons: *** [build\release\wolf\code\client\cl_cin.obj] Error 1
scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\client\cl_cin.cpp(N,N): error: cannot decrement expression of type bool |
| 1 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' with an lvalue of type 'file': different return type ('file' vs 'file') |
| 1 | scons: *** [build\release\wolf\code\client\cl_cin.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error N |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code\client\cl_cgame.cpp(N,N): warning: result of comparison of constant N with expression of type 'file' is always false [-Wtautological-constant-out-of-range-compare] |

