# Build log digest

- Log: `build.log`
- Lines: 56
- Errors: 8
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\cgame\../game/q_shared.h(1,1): error: unknown type name 'successfully'
): error: expected ';' after top level declarator
code\game\cgame\../game/q_shared.h(1,24): error: expected ';' after top level declarator
): error: expected ';' after top level declarator
scons: *** [build\release\wolf\code\client\cl_input.obj] Error 1
scons: *** [build\release\wolf\code\client\cl_console.obj] Error 1
scons: *** [build\release\wolf\code\client\cl_cin.obj] Error 1
scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | ): error: expected ';' after top level declarator |
| 1 | code\game\cgame\../game/q_shared.h(N,N): error: unknown type name 'successfully' |
| 1 | code\game\cgame\../game/q_shared.h(N,N): error: expected ';' after top level declarator |
| 1 | scons: *** [build\release\wolf\code\client\cl_input.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\client\cl_console.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\client\cl_cin.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\client\cl_cgame.obj] Error N |

