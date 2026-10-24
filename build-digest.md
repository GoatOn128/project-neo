# Build log digest

- Log: `build.log`
- Lines: 153
- Errors: 4
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
tr_shader.obj : error LNK2019: unresolved external symbol "char * __cdecl CopyString(char const *)" (?CopyString@@YAPADPBD@Z) referenced in function "int __cdecl ParseShader(char * *)" (?ParseShader@@YAHPAPAD@Z)
tr_sky.obj : error LNK2019: unresolved external symbol "float __cdecl Q_acos(float)" (?Q_acos@@YAMM@Z) referenced in function "void __cdecl R_InitSkyTexCoords(float)" (?R_InitSkyTexCoords@@YAXM@Z)
bin\base\ref_trin_glx86.dll : fatal error LNK1120: 2 unresolved externals
scons: *** [bin\base\ref_trin_glx86.dll] Error 1120
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | tr_shader.obj : error LNKN: unresolved external symbol 'file' (?CopyString@@YAPADPBD@Z) referenced in function 'file' (?ParseShader@@YAHPAPAD@Z) |
| 1 | tr_sky.obj : error LNKN: unresolved external symbol 'file' (?Q_acos@@YAMM@Z) referenced in function 'file' (?R_InitSkyTexCoords@@YAXM@Z) |
| 1 | bin\base\ref_trin_glxN.dll : fatal error LNKN: N unresolved externals |
| 1 | scons: *** [bin\base\ref_trin_glxN.dll] Error N |

