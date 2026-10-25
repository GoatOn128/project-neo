# Build log digest

- Log: `build.log`
- Lines: 125
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\cgame\cg_predict.cpp(619,29): error: no member named 'venomTime' in 'playerState_s'
code\game\cgame\cg_predict.cpp(619,56): error: no member named 'venomTime' in 'playerState_s'
scons: *** [build\release\cgame\code\game\cgame\cg_predict.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | code\game\cgame\cg_predict.cpp(N,N): error: no member named 'file' in 'file' |
| 1 | scons: *** [build\release\cgame\code\game\cgame\cg_predict.obj] Error N |

