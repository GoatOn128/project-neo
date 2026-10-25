# Build log digest

- Log: `build.log`
- Lines: 348
- Errors: 6
- Warnings: 3

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\botlib\be_aas_reach.cpp(1870,62): error: expected '}'
code\game\botlib\be_aas_reach.cpp(1870,62): error: expected '}'
code\game\botlib\be_aas_reach.cpp(1870,62): error: expected '}'
code\game\botlib\be_aas_reach.cpp(1870,62): error: expected '}'
code\game\botlib\be_aas_reach.cpp(1870,62): error: expected '}'
scons: *** [build\release\aasbuild\code\game\botlib\be_aas_reach.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 5 | code\game\botlib\be_aas_reach.cpp(N,N): error: expected 'file' |
| 1 | scons: *** [build\release\aasbuild\code\game\botlib\be_aas_reach.obj] Error N |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 2 | code\game\game\g_mover.cpp(N,N): warning: result of comparison of constant N with expression of type 'file' is always false [-Wtautological-constant-out-of-range-compare] |
| 1 | code\game\ui\ui_main.cpp(N,N): warning: result of comparison of constant N with expression of type 'file' is always false [-Wtautological-constant-out-of-range-compare] |

