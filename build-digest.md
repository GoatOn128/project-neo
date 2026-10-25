# Build log digest

- Log: `build.log`
- Lines: 416
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
aasbuild.lib(be_aas_move.obj) : error LNK2019: unresolved external symbol "int __cdecl VectorCompare(float const * const,float const * const)" (?VectorCompare@@YAHQBM0@Z) referenced in function "void __cdecl AAS_SetMovedir(float * const,float * const)" (?AAS_SetMovedir@@YAXQAM0@Z)
bin\toolsx86.dll : fatal error LNK1120: 1 unresolved externals
scons: *** [bin\toolsx86.dll] Error 1120
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | aasbuild.lib(be_aas_move.obj) : error LNKN: unresolved external symbol 'file' (?VectorCompare@@YAHQBMN@Z) referenced in function 'file' (?AAS_SetMovedir@@YAXQAMN@Z) |
| 1 | bin\toolsxN.dll : fatal error LNKN: N unresolved externals |
| 1 | scons: *** [bin\toolsxN.dll] Error N |

