# Build log digest

- Log: `build.log`
- Lines: 73
- Errors: 5
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\sys\sys_events.cpp(459,5): error: no matching function for call to 'Sys_QueEvent'
code\sys\sys_events.cpp(460,5): error: no matching function for call to 'Sys_QueEvent'
code\sys\sys_events.cpp(462,5): error: no matching function for call to 'Sys_QueEvent'
code\sys\sys_events.cpp(463,5): error: no matching function for call to 'Sys_QueEvent'
scons: *** [build\release\wolf\code\sys\sys_events.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 4 | code\sys\sys_events.cpp(N,N): error: no matching function for call to 'file' |
| 1 | scons: *** [build\release\wolf\code\sys\sys_events.obj] Error N |

