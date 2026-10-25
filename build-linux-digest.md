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

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 34 | code/game/cgame/cg_local.h:N: warning: "CHAR_WIDTH" redefined |
| 14 | code/game/botlib/be_aas_main.h:N:N: warning: unnecessary parentheses in declaration of 'aasworld' [-Wparentheses] |
| 8 | code/game/cgame/fx/../cg_local.h:N: warning: "CHAR_WIDTH" redefined |
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

