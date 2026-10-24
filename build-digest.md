# Build log digest

- Log: `build.log`
- Lines: 152
- Errors: 11
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\cgame\cg_main.cpp(107,2): error: cannot initialize a member subobject of type 'qboolean (*)()' (aka 'int (*)()') with an lvalue of type 'bool ()': different return type ('qboolean' (aka 'int') vs 'bool')
code\game\cgame\cg_main.cpp(108,2): error: cannot initialize a member subobject of type 'void (*)(int, stereoFrame_t, qboolean)' (aka 'void (*)(int, stereoFrame_t, int)') with an lvalue of type 'void (int, stereoFrame_t, bool)': type mismatch at 3rd parameter ('qboolean' (aka 'int') vs 'bool')
code\game\cgame\cg_main.cpp(111,2): error: cannot initialize a member subobject of type 'void (*)(int, qboolean)' (aka 'void (*)(int, int)') with an lvalue of type 'void (int, bool)': type mismatch at 2nd parameter ('qboolean' (aka 'int') vs 'bool')
code\game\cgame\cg_main.cpp(114,2): error: cannot initialize a member subobject of type 'qboolean (*)(int, char *, orientation_t *)' (aka 'int (*)(int, char *, orientation_t *)') with an lvalue of type 'bool (int, char *, orientation_t *)': different return type ('qboolean' (aka 'int') vs 'bool')
code\game\cgame\cg_main.cpp(2218,26): error: incompatible function pointer types assigning to 'qboolean (*)(int)' (aka 'int (*)(int)') from 'bool (*)(int)'
code\game\cgame\cg_main.cpp(2228,28): error: incompatible function pointer types assigning to 'qboolean (*)(int, int, float *, int)' (aka 'int (*)(int, int, float *, int)') from 'bool (*)(int, int, float *, int)'
scons: *** [build\release\cgame\code\game\cgame\cg_main.obj] Error 1
code\game\cgame\cg_newDraw.cpp(1464,10): error: functions that differ only in their return type cannot be overloaded
code\game\cgame\cg_newDraw.cpp(1490,10): error: functions that differ only in their return type cannot be overloaded
code\game\cgame\cg_newDraw.cpp(1518,10): error: functions that differ only in their return type cannot be overloaded
scons: *** [build\release\cgame\code\game\cgame\cg_newDraw.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 3 | code\game\cgame\cg_newDraw.cpp(N,N): error: functions that differ only in their return type cannot be overloaded |
| 2 | code\game\cgame\cg_main.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file': different return type ('file' (aka 'file') vs 'file') |
| 2 | code\game\cgame\cg_main.cpp(N,N): error: incompatible function pointer types assigning to 'file' (aka 'file') from 'file' |
| 1 | code\game\cgame\cg_main.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file': type mismatch at Nrd parameter ('file' (aka 'file') vs 'file') |
| 1 | code\game\cgame\cg_main.cpp(N,N): error: cannot initialize a member subobject of type 'file' (aka 'file') with an lvalue of type 'file': type mismatch at Nnd parameter ('file' (aka 'file') vs 'file') |
| 1 | scons: *** [build\release\cgame\code\game\cgame\cg_main.obj] Error N |
| 1 | scons: *** [build\release\cgame\code\game\cgame\cg_newDraw.obj] Error N |

