# Build log digest

- Log: `build.log`
- Lines: 106
- Errors: 3
- Warnings: 1

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\renderer\tr_vfx.cpp(584,29): error: use of undeclared identifier 'GL_TEXTURE0_ARB'
code\renderer\tr_vfx.cpp(586,29): error: use of undeclared identifier 'GL_TEXTURE0_ARB'
scons: *** [build\release\renderer\code\renderer\tr_vfx.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | code\renderer\tr_vfx.cpp(N,N): error: use of undeclared identifier 'file' |
| 1 | scons: *** [build\release\renderer\code\renderer\tr_vfx.obj] Error N |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code\sys\sys_glimp.cpp(N,N): warning: unused variable 'file' [-Wunused-variable] |

