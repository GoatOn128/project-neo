# Build log digest

- Log: `build-linux.log`
- Lines: 1717
- Errors: 20
- Warnings: 219

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code/renderer/tr_model.cpp:1729:33: error: invalid conversion from 'void*' to 'byte*' {aka 'unsigned char*'} [-fpermissive]
scons: *** [build/release/renderer/code/renderer/tr_model.os] Error 1
code/renderer/tr_glprogs.cpp:50:16: error: 'PROC' was not declared in this scope
code/renderer/tr_glprogs.cpp:57:5: error: 'PROC' was not declared in this scope
code/renderer/tr_glprogs.cpp:58:20: error: 'address' was not declared in this scope
code/renderer/tr_glprogs.cpp:58:27: error: '{anonymous}::ValidProc' cannot be used as a function
code/renderer/tr_glprogs.cpp:59:25: error: expected ';' before 'SDL_GL_GetProcAddress'
code/renderer/tr_glprogs.cpp:61:29: error: 'address' was not declared in this scope
code/renderer/tr_glprogs.cpp:61:36: error: '{anonymous}::ValidProc' cannot be used as a function
scons: *** [build/release/renderer/code/renderer/tr_glprogs.os] Error 1
code/game/cgame/fx/genericparser2.cpp:261:20: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'?
code/game/cgame/fx/genericparser2.cpp:418:17: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'?
code/game/cgame/fx/genericparser2.cpp:510:20: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'?
code/game/cgame/fx/genericparser2.cpp:518:13: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'?
code/game/cgame/fx/genericparser2.cpp:579:13: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'?
scons: *** [build/release/cgame/code/game/cgame/fx/genericparser2.os] Error 1
collect2: error: ld returned 1 exit status
scons: *** [bin/base/qagamex64.so] Error 1
collect2: error: ld returned 1 exit status
scons: *** [bin/base/uix64.so] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 5 | code/game/cgame/fx/genericparserN.cpp:N:N: error: 'strcmpi' was not declared in this scope; did you mean 'strcmp'? |
| 2 | code/renderer/tr_glprogs.cpp:N:N: error: 'PROC' was not declared in this scope |
| 2 | code/renderer/tr_glprogs.cpp:N:N: error: 'address' was not declared in this scope |
| 2 | code/renderer/tr_glprogs.cpp:N:N: error: '{anonymous}::ValidProc' cannot be used as a function |
| 2 | collectN: error: ld returned N exit status |
| 1 | code/renderer/tr_model.cpp:N:N: error: invalid conversion from 'void*' to 'byte*' {aka 'unsigned char*'} [-fpermissive] |
| 1 | scons: *** [build/release/renderer/code/renderer/tr_model.os] Error N |
| 1 | code/renderer/tr_glprogs.cpp:N:N: error: expected ';' before 'SDL_GL_GetProcAddress' |
| 1 | scons: *** [build/release/renderer/code/renderer/tr_glprogs.os] Error N |
| 1 | scons: *** [build/release/cgame/code/game/cgame/fx/genericparserN.os] Error N |
| 1 | scons: *** [bin/base/qagamexN.so] Error N |
| 1 | scons: *** [bin/base/uixN.so] Error N |

## Errors by file

| Count | File |
| --- | --- |
| 7 | code/renderer/tr_glprogs.cpp |
| 5 | code/game/cgame/fx/genericparser2.cpp |
| 1 | code/renderer/tr_model.cpp |

## Warnings, first 40

```
code/game/cgame/../game/q_shared.h:605:43: warning: value computed is not used [-Wunused-value]
code/qcommon/cm_load.cpp:663:25: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/cm_patch.cpp:1400:25: warning: 'bestplane[0]' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/cm_patch.cpp:1400:25: warning: 'bestplane[1]' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/cm_patch.cpp:1400:25: warning: 'bestplane[2]' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/cm_patch.cpp:1400:25: warning: 'bestplane[3]' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/cm_trace.cpp:152:14: warning: 'y' is used uninitialized [-Wuninitialized]
code/qcommon/cm_trace.cpp:152:14: warning: array subscript 'long int[0]' is partly outside array bounds of 'float [1]' [-Warray-bounds=]
code/qcommon/cmd.cpp:342:28: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/qcommon/common.cpp:150:62: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/common.cpp:1357:25: warning: comparison of integer expressions of different signedness: 'int' and 'unsigned int' [-Wsign-compare]
code/qcommon/common.cpp:1700:52: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/qcommon/cvar.cpp:858:39: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/qcommon/files.cpp:553:38: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:566:39: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:600:38: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:613:39: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:1057:39: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:1063:39: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare]
code/qcommon/files.cpp:1869:24: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/files.cpp:1886:24: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/files.cpp:1910:24: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/msg.cpp:448:21: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/msg.cpp:472:21: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/msg.cpp:495:21: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/net_chan.cpp:418:64: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/game/game/q_math.cpp:512:14: warning: 'y' is used uninitialized [-Wuninitialized]
code/game/game/q_math.cpp:512:14: warning: array subscript 'long int[0]' is partly outside array bounds of 'float [1]' [-Warray-bounds=]
code/game/game/q_shared.cpp:997:18: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/server/sv_main.cpp:86:26: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/server/sv_main.cpp:369:58: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/qcommon/unzip.cpp:179:14: warning: ignoring return value of 'size_t fread(void*, size_t, size_t, FILE*)' declared with attribute 'warn_unused_result' [-Wunused-result]
code/qcommon/unzip.cpp:150:14: warning: ignoring return value of 'size_t fread(void*, size_t, size_t, FILE*)' declared with attribute 'warn_unused_result' [-Wunused-result]
code/renderer/tr_backend.cpp:933:37: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/renderer/tr_bsp.cpp:1821:57: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/renderer/tr_bsp.cpp:1828:42: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare]
code/renderer/tr_bsp.cpp:2217:25: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare]
code/renderer/tr_bsp.cpp:197:59: warning: 'out[0]' may be used uninitialized [-Wmaybe-uninitialized]
code/renderer/tr_bsp.cpp:198:59: warning: 'out[1]' may be used uninitialized [-Wmaybe-uninitialized]
code/renderer/tr_bsp.cpp:199:59: warning: 'out[2]' may be used uninitialized [-Wmaybe-uninitialized]
```

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 34 | code/game/cgame/cg_local.h:N: warning: "CHAR_WIDTH" redefined   [previously defined at /usr/include/limits.h] |
| 14 | code/game/botlib/be_aas_main.h:N:N: warning: unnecessary parentheses in declaration of 'aasworld' [-Wparentheses] |
| 8 | code/game/cgame/fx/../cg_local.h:N: warning: "CHAR_WIDTH" redefined   [previously defined at /usr/include/limits.h] |
| 8 | code/game/cgame/fx/genericparserN.h:N: warning: ignoring '#pragma warning ' [-Wunknown-pragmas] |
| 6 | code/qcommon/files.cpp:N:N: warning: comparison of integer expressions of different signedness: 'size_t' {aka 'long unsigned int'} and 'int' [-Wsign-compare] |
| 6 | code/splines/math_matrix.h:N:N: warning: 'void* memcpy(void*, const void*, size_t)' writing to an object of type 'class idVecN' with no trivial copy-assignment; use copy-assignment or copy-initialization instead [-Wclass-memaccess] |
| 5 | code/game/game/q_math.cpp:N:N: warning: 'y' is used uninitialized [-Wuninitialized] |
| 5 | code/game/game/q_math.cpp:N:N: warning: array subscript 'long int[N]' is partly outside array bounds of 'float [N]' [-Warray-bounds=] |
| 5 | code/game/game/q_shared.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare] |
| 4 | code/qcommon/cm_patch.cpp:N:N: warning: 'bestplane[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 4 | code/sound/opl/doom_opl.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'unsigned int' [-Wsign-compare] |
| 3 | code/qcommon/files.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare] |
| 3 | code/qcommon/msg.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare] |
| 3 | code/renderer/tr_bsp.cpp:N:N: warning: 'out[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/cgame/../game/q_shared.h:N:N: warning: 'fogDepthVector[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | ccN: warning: command-line option '-fno-operator-names' is valid for C++/ObjC++ but not for C |
| 3 | code/game/cgame/cg_ents.cpp:N:N: warning: the address of 'centity_s::boltCrawlDirs' will never be NULL [-Waddress] |
| 3 | code/game/cgame/cg_doommap.cpp:N:N: warning: comparison of integer expressions of different signedness: 'const intN_t' {aka 'const int'} and 'long unsigned int' [-Wsign-compare] |
| 3 | code/game/game/g_save.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare] |
| 3 | code/game/botlib/be_aas_reach.cpp:N:N: warning: 'water_bestend[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/be_aas_reach.cpp:N:N: warning: 'water_bestnormal[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/be_aas_reach.cpp:N:N: warning: 'ground_beststart[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/../game/q_shared.h:N:N: warning: 'ground_bestend[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/../game/q_shared.h:N:N: warning: 'ground_bestnormal[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/be_aas_reach.cpp:N:N: warning: 'water_beststart[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 3 | code/game/botlib/l_precomp.cpp:N:N: warning: 'char* __builtin_strncpy(char*, const char*, long unsigned int)' specified bound N equals destination size [-Wstringop-truncation] |
| 3 | code/sound/opl/doom_opl.cpp:N:N: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare] |
| 2 | code/server/sv_main.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'long unsigned int' [-Wsign-compare] |
| 2 | code/qcommon/unzip.cpp:N:N: warning: ignoring return value of 'size_t fread(void*, size_t, size_t, FILE*)' declared with attribute 'warn_unused_result' [-Wunused-result] |
| 2 | code/renderer/tr_bsp.cpp:N:N: warning: comparison of integer expressions of different signedness: 'unsigned int' and 'int' [-Wsign-compare] |

## Warnings by file

| Count | File |
| --- | --- |
| 34 | code/game/cgame/cg_local.h |
| 14 | code/game/botlib/be_aas_main.h |
| 14 | code/game/botlib/be_aas_reach.cpp |
| 10 | code/game/game/q_math.cpp |
| 9 | code/qcommon/files.cpp |
| 8 | code/game/cgame/fx/../cg_local.h |
| 8 | code/game/cgame/fx/genericparser2.h |
| 7 | code/sound/opl/doom_opl.cpp |
| 6 | code/renderer/tr_bsp.cpp |
| 6 | code/game/botlib/../game/q_shared.h |
| 6 | code/splines/math_matrix.h |
| 5 | code/game/game/q_shared.cpp |
| 5 | code/game/game/g_save.cpp |
| 4 | code/game/cgame/../game/q_shared.h |
| 4 | code/qcommon/cm_patch.cpp |
| 4 | code/renderer/tr_shade.cpp |
| 4 | code/game/game/bg_animation.cpp |
| 4 | code/game/cgame/cg_ents.cpp |
| 3 | code/qcommon/common.cpp |
| 3 | code/qcommon/msg.cpp |
| 3 | code/renderer/tr_shade_calc.cpp |
| 3 | code/game/cgame/cg_doommap.cpp |
| 3 | code/game/game/g_main.cpp |
| 3 | code/game/botlib/be_ai_chat.cpp |
| 3 | code/game/botlib/l_precomp.cpp |
| 2 | code/qcommon/cm_trace.cpp |
| 2 | code/server/sv_main.cpp |
| 2 | code/qcommon/unzip.cpp |
| 2 | code/game/cgame/cg_consolecmds.cpp |
| 2 | code/game/ui/ui_shared.cpp |
| 2 | code/game/game/g_client.cpp |
| 2 | code/game/game/g_combat.cpp |
| 2 | code/game/game/g_team.cpp |
| 2 | code/game/game/g_doombots_goals.cpp |
| 2 | /usr/include/x86_64-linux-gnu/bits/string_fortified.h |
| 2 | code/game/botlib/be_ai_goal.cpp |
| 1 | code/qcommon/cm_load.cpp |
| 1 | code/qcommon/cmd.cpp |
| 1 | code/qcommon/cvar.cpp |
| 1 | code/qcommon/net_chan.cpp |
| 1 | code/renderer/tr_backend.cpp |
| 1 | code/renderer/tr_image.cpp |
| 1 | code/renderer/tr_main.cpp |
| 1 | code/renderer/tr_model.cpp |
| 1 | code/game/cgame/cg_main.cpp |
| 1 | code/game/cgame/cg_players.cpp |
| 1 | code/game/cgame/cg_playerstate.cpp |
| 1 | code/game/cgame/cg_servercmds.cpp |
| 1 | code/game/cgame/cg_weapons.cpp |
| 1 | code/game/cgame/fx/FxScheduler.cpp |
| 1 | code/game/game/g_cmds.cpp |
| 1 | code/game/ui/ui_main.cpp |
| 1 | code/game/ui/ui_players.cpp |
| 1 | code/game/botlib/be_aas_main.cpp |
| 1 | code/game/botlib/be_ai_char.cpp |
| 1 | code/game/botlib/be_ai_weap.cpp |
| 1 | code/game/botlib/l_log.cpp |

