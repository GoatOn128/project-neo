# Build log digest

- Log: `build.log`
- Lines: 426
- Errors: 12
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
cl_cin.obj : error LNK2019: unresolved external symbol "struct glconfig_t glConfig" (?glConfig@@3Uglconfig_t@@A) referenced in function "void __cdecl RoQInterrupt(void)" (?RoQInterrupt@@YAXXZ)
sys_events.obj : error LNK2019: unresolved external symbol "struct cvar_s * r_fullscreen" (?r_fullscreen@@3PAUcvar_s@@A) referenced in function "void __cdecl Sys_EventLoop(void)" (?Sys_EventLoop@@YAXXZ)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglColor3fv)(float const *)" (?qglColor3fv@@3P6GXPBM@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglPointSize)(float)" (?qglPointSize@@3P6GXM@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglBegin)(unsigned int)" (?qglBegin@@3P6GXI@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglVertex3fv)(float const *)" (?qglVertex3fv@@3P6GXPBM@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglEnd)(void)" (?qglEnd@@3P6GXXZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglRasterPos3fv)(float const *)" (?qglRasterPos3fv@@3P6GXPBM@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglCallLists)(int,unsigned int,void const *)" (?qglCallLists@@3P6GXHIPBX@ZA) referenced in function "void __cdecl glLabeledPoint(class idVec3 &,class idVec3 &,float,char const *)" (?glLabeledPoint@@YAXAAVidVec3@@0MPBD@Z)
splines.lib(splines.obj) : error LNK2019: unresolved external symbol "void (__stdcall* qglVertex3f)(float,float,float)" (?qglVertex3f@@3P6GXMMM@ZA) referenced in function "void __cdecl glBox(class idVec3 &,class idVec3 &,float)" (?glBox@@YAXAAVidVec3@@0M@Z)
bin\Doom.exe : fatal error LNK1120: 10 unresolved externals
scons: *** [bin\Doom.exe] Error 1120
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | cl_cin.obj : error LNKN: unresolved external symbol 'file' (?glConfig@@NUglconfig_t@@A) referenced in function 'file' (?RoQInterrupt@@YAXXZ) |
| 1 | sys_events.obj : error LNKN: unresolved external symbol 'file' (?r_fullscreen@@NPAUcvar_s@@A) referenced in function 'file' (?Sys_EventLoop@@YAXXZ) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglColorNfv@@NPNGXPBM@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglPointSize@@NPNGXM@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglBegin@@NPNGXI@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglVertexNfv@@NPNGXPBM@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglEnd@@NPNGXXZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglRasterPosNfv@@NPNGXPBM@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglCallLists@@NPNGXHIPBX@ZA) referenced in function 'file' (?glLabeledPoint@@YAXAAVidVecN@@NMPBD@Z) |
| 1 | splines.lib(splines.obj) : error LNKN: unresolved external symbol 'file' (?qglVertexNf@@NPNGXMMM@ZA) referenced in function 'file' (?glBox@@YAXAAVidVecN@@NM@Z) |
| 1 | bin\Doom.exe : fatal error LNKN: N unresolved externals |
| 1 | scons: *** [bin\Doom.exe] Error N |

