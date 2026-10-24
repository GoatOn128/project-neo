# Build log digest

- Log: `build.log`
- Lines: 26
- Errors: 2
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_ui.cpp(793,2): error: cannot initialize a member subobject of type 'void (*)(void *, int, fileHandle_t)' (aka 'void (*)(void *, int, int)') with an lvalue of type 'int (void *, int, fileHandle_t)' (aka 'int (void *, int, int)'): different return type ('void' vs 'int')
scons: *** [build\release\wolf\code\client\cl_ui.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\client\cl_ui.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file' (aka 'file'): different return type ('file' vs 'file') |
| 1 | scons: *** [build\release\wolf\code\client\cl_ui.obj] Error N |

