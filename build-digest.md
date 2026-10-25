# Build log digest

- Log: `build.log`
- Lines: 384
- Errors: 3
- Warnings: 1

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\game\botlib\be_aas_route.cpp(1764,64): error: expected '}'
code\game\botlib\be_aas_route.cpp(1764,64): error: expected '}'
scons: *** [build\release\botlib\code\game\botlib\be_aas_route.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | code\game\botlib\be_aas_route.cpp(N,N): error: expected 'file' |
| 1 | scons: *** [build\release\botlib\code\game\botlib\be_aas_route.obj] Error N |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code\game\botlib\be_aas_entity.cpp(N,N): warning: result of comparison of constant N with expression of type 'file' is always false [-Wtautological-constant-compare] |

