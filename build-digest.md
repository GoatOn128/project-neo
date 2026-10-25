# Build log digest

- Log: `build.log`
- Lines: 65
- Errors: 11
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\client\cl_cgame.cpp(462,2): error: no viable conversion from '(lambda at code/client/cl_cgame.cpp:462:2)' to 'void (*)(bool)'
code\client\cl_cgame.cpp(491,2): error: no viable conversion from '(lambda at code/client/cl_cgame.cpp:491:2)' to 'bool (*)(qhandle_t, const char *, char *)' (aka 'bool (*)(int, const char *, char *)')
code\client\cl_cgame.cpp(515,2): error: cannot initialize a member subobject of type 'bool (*)(int, snapshot_t *)' with an lvalue of type 'qboolean (int, snapshot_t *)' (aka 'int (int, snapshot_t *)'): different return type ('bool' vs 'qboolean' (aka 'int'))
code\client\cl_cgame.cpp(516,2): error: cannot initialize a member subobject of type 'bool (*)(int)' with an lvalue of type 'qboolean (int)' (aka 'int (int)'): different return type ('bool' vs 'qboolean' (aka 'int'))
code\client\cl_cgame.cpp(518,2): error: cannot initialize a member subobject of type 'bool (*)(int, usercmd_t *)' (aka 'bool (*)(int, struct usercmd_s *)') with an lvalue of type 'qboolean (int, usercmd_t *)' (aka 'int (int, struct usercmd_s *)'): different return type ('bool' vs 'qboolean' (aka 'int'))
code\client\cl_cgame.cpp(521,2): error: cannot initialize a member subobject of type 'bool (*)(int, const char *)' with an lvalue of type 'int (int, const char *)': different return type ('bool' vs 'int')
code\client\cl_cgame.cpp(533,2): error: no viable conversion from '(lambda at code/client/cl_cgame.cpp:533:2)' to 'bool (*)(int, int, vec3_t *, vec3_t *, float *)' (aka 'bool (*)(int, int, float (*)[3], float (*)[3], float *)')
code\client\cl_cgame.cpp(534,2): error: cannot initialize a member subobject of type 'bool (*)(int)' with an lvalue of type 'qboolean (int)' (aka 'int (int)'): different return type ('bool' vs 'qboolean' (aka 'int'))
code\client\cl_cgame.cpp(552,2): error: no viable conversion from '(lambda at code/client/cl_cgame.cpp:552:2)' to 'bool (*)(char *, int)'
code\client\cl_cgame.cpp(577,2): error: no viable conversion from '(lambda at code/client/cl_cgame.cpp:577:2)' to 'bool (*)(int, char *, void **)'
scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 3 | code\client\cl_cgame.cpp(N,N): error: no viable conversion from 'file' to 'file' |
| 3 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' with an lvalue of type 'file' (aka 'file'): different return type ('file' vs 'file' (aka 'file')) |
| 2 | code\client\cl_cgame.cpp(N,N): error: no viable conversion from 'file' to 'file' (aka 'file') |
| 1 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file' (aka 'file'): different return type ('file' vs 'file' (aka 'file')) |
| 1 | code\client\cl_cgame.cpp(N,N): error: cannot initialize a member subobject of type 'file' with an lvalue of type 'file': different return type ('file' vs 'file') |
| 1 | scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error N |

