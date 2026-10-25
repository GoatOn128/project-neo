# Build log digest

- Log: `build.log`
- Lines: 355
- Errors: 5
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\botlib\l_precomp.cpp(989,4): error: use of undeclared identifier 'Q_strcat'
code\game\botlib\l_precomp.cpp(2951,2): error: use of undeclared identifier 'Q_strncpyz'; did you mean 'strncpy'?
code\game\botlib\l_precomp.cpp(2989,2): error: use of undeclared identifier 'Q_strncpyz'; did you mean 'strncpy'?
code\game\botlib\l_precomp.cpp(3023,2): error: use of undeclared identifier 'Q_strncpyz'; did you mean 'strncpy'?
scons: *** [build\release\aasbuild\code\game\botlib\l_precomp.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 3 | code\game\botlib\l_precomp.cpp(N,N): error: use of undeclared identifier 'Q_strncpyz'; did you mean 'strncpy'? |
| 1 | code\game\botlib\l_precomp.cpp(N,N): error: use of undeclared identifier 'Q_strcat' |
| 1 | scons: *** [build\release\aasbuild\code\game\botlib\l_precomp.obj] Error N |

