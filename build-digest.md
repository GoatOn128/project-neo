# Build log digest

- Log: `build.log`
- Lines: 63
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\server\sv_game.cpp(309,2): error: no viable conversion from '(lambda at code/server/sv_game.cpp:309:2)' to 'int (*)(const char *, const char *)'
code\server\sv_game.cpp(343,2): error: no viable conversion from '(lambda at code/server/sv_game.cpp:343:2)' to 'qboolean (*)(char *, int)' (aka 'int (*)(char *, int)')
scons: *** [build\release\wolf\code\server\sv_game.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\server\sv_game.cpp(N,N): error: no viable conversion from 'file' to 'file' |
| 1 | code\server\sv_game.cpp(N,N): error: no viable conversion from 'file' to 'file' (aka 'file') |
| 1 | scons: *** [build\release\wolf\code\server\sv_game.obj] Error N |

