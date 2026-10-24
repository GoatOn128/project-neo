# Build log digest

- Log: `build.log`
- Lines: 20
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_cgame.cpp(440,2): error: cannot initialize a member subobject of type 'void (*)(void *, int, fileHandle_t)' (aka 'void (*)(void *, int, int)') with an lvalue of type 'int (void *, int, fileHandle_t)' (aka 'int (void *, int, int)'): different return type ('void' vs 'int')
code\client\cl_cgame.cpp(533,2): error: cannot initialize a member subobject of type 'qboolean (*)(int, int, vec3_t *, vec3_t *, float *)' (aka 'int (*)(int, int, float (*)[3], float (*)[3], float *)') with an lvalue of type 'int (int, int, float *, float *, float *)': type mismatch at 3rd parameter ('vec3_t *' (aka 'float (*)[3]') vs 'float *')
scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file' (aka 'file'): different return type ('file' vs 'file') |
| 1 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file': type mismatch at Nrd parameter ('file' (aka 'file') vs 'file') |
| 1 | scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error N |

