# Build log digest

- Log: `build-linux.log`
- Lines: 24948
- Errors: 1184
- Warnings: 6864

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code/client/../sys/win32/win_local.h:35:10: fatal error: windows.h: No such file or directory
scons: *** [build/release/wolf/code/client/cl_main.o] Error 1
code/sys/unix/unix_net.cpp:318:35: error: 'F_GETFL' was not declared in this scope
code/sys/unix/unix_net.cpp:318:17: error: 'fcntl' was not declared in this scope
code/sys/unix/unix_net.cpp:319:47: error: 'F_SETFL' was not declared in this scope
code/sys/unix/unix_net.cpp:319:64: error: 'O_NONBLOCK' was not declared in this scope; did you mean 'SOCK_NONBLOCK'?
scons: *** [build/release/wolf/code/sys/unix/unix_net.o] Error 1
code/sys/unix/../sys_local.h:57:33: error: 'sysEventType_t' has not been declared
code/sys/unix/../sys_local.h:72:24: error: 'netadr_t' was not declared in this scope
code/sys/unix/../sys_local.h:72:34: error: 'net_from' was not declared in this scope
code/sys/unix/../sys_local.h:72:44: error: 'msg_t' was not declared in this scope
code/sys/unix/../sys_local.h:72:51: error: 'net_message' was not declared in this scope
code/sys/unix/../sys_local.h:72:63: error: expression list treated as compound expression in initializer [-fpermissive]
scons: *** [build/release/wolf/code/sys/unix/unix_shared.o] Error 1
code/sys/../tools/editor/EditorAPI.h:34:47: error: expected ';' before '(' token
code/sys/sys_editor.cpp:83:42: error: 'DoomEditorGetAPI' does not name a type; did you mean 'DoomEditorAPI'?
scons: *** [build/release/wolf/code/sys/sys_editor.o] Error 1
code/renderer/tr_local.h:536:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:929:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:1002:33: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:1237:100: error: expected ',' or '...' before 'or' token
code/renderer/tr_local.h:1474:69: error: expected ',' or '...' before 'or' token
code/renderer/tr_animation.cpp:94:38: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:40: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:94:63: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:65: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:94:38: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:40: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:94:63: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:65: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:94:38: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:40: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:94:63: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:94:65: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 209 | code/renderer/tr_main.cpp:N:N: error: expected primary-expression before 'file' token |
| 127 | code/renderer/tr_main.cpp:N:N: error: expected unqualified-id before 'file' token |
| 93 | code/renderer/tr_local.h:N:N: error: expected unqualified-id before 'file' token |
| 80 | code/game/cgame/fx/../../game/q_shared.h:N:N: error: expected unqualified-id before 'file' token |
| 63 | code/renderer/tr_shade_calc.cpp:N:N: error: expected unqualified-id before 'file' token |
| 63 | code/renderer/tr_shade_calc.cpp:N:N: error: expected primary-expression before 'file' token |
| 62 | code/renderer/tr_local.h:N:N: error: expected 'file' or 'file' before 'file' token |
| 54 | code/renderer/tr_surface.cpp:N:N: error: expected unqualified-id before 'file' token |
| 54 | code/renderer/tr_surface.cpp:N:N: error: expected primary-expression before 'file' token |
| 25 | code/renderer/tr_light.cpp:N:N: error: expected primary-expression before 'file' token |
| 19 | code/renderer/tr_backend.cpp:N:N: error: expected unqualified-id before 'file' token |
| 19 | code/renderer/tr_backend.cpp:N:N: error: expected primary-expression before 'file' token |
| 18 | code/renderer/tr_sky.cpp:N:N: error: expected unqualified-id before 'file' token |
| 18 | code/renderer/tr_sky.cpp:N:N: error: expected primary-expression before 'file' token |
| 17 | code/renderer/tr_vfx.cpp:N:N: error: expected unqualified-id before 'file' token |
| 17 | code/renderer/tr_vfx.cpp:N:N: error: expected primary-expression before 'file' token |
| 13 | code/renderer/tr_doom.cpp:N:N: error: expected unqualified-id before 'file' token |
| 13 | code/renderer/tr_doom.cpp:N:N: error: expected primary-expression before 'file' token |
| 12 | code/renderer/tr_scene.cpp:N:N: error: expected unqualified-id before 'file' token |
| 12 | code/renderer/tr_scene.cpp:N:N: error: expected primary-expression before 'file' token |
| 10 | code/game/cgame/fx/FxTemplate.cpp:N:N: error: 'file' was not declared in this scope; did you mean 'file'? |
| 9 | code/renderer/tr_animation.cpp:N:N: error: expected unqualified-id before 'file' token |
| 9 | code/renderer/tr_animation.cpp:N:N: error: expected primary-expression before 'file' token |
| 9 | code/renderer/tr_cmesh.cpp:N:N: error: expected unqualified-id before 'file' token |
| 9 | code/renderer/tr_cmesh.cpp:N:N: error: expected primary-expression before 'file' token |
| 9 | code/renderer/tr_mesh.cpp:N:N: error: expected unqualified-id before 'file' token |
| 9 | code/renderer/tr_mesh.cpp:N:N: error: expected primary-expression before 'file' token |
| 6 | code/renderer/tr_decals.cpp:N:N: error: expected unqualified-id before 'file' token |
| 6 | code/renderer/tr_decals.cpp:N:N: error: expected primary-expression before 'file' token |
| 6 | code/game/cgame/fx/FxScheduler.h:N:N: error: 'file' was not declared in this scope; did you mean 'file'? |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 2643 | code/game/game/bg_misc.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 536 | code/client/cl_keys.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 522 | code/game/ui/ui_shared.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 303 | code/game/cgame/cg_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 223 | code/game/ui/ui_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 220 | code/game/game/bg_animation.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 199 | code/game/game/g_spawn.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 148 | code/qcommon/msg.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 136 | code/game/game/g_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 131 | code/game/botlib/be_ai_chat.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 115 | code/game/botlib/l_precomp.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 107 | code/game/cgame/cg_players.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 85 | code/game/botlib/be_aas_reach.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 79 | code/game/botlib/l_script.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 78 | code/game/game/g_combat.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 66 | code/game/cgame/cg_event.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 64 | code/game/botlib/be_ai_weap.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 61 | code/renderer/tr_shader.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 58 | code/game/botlib/be_ai_goal.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 51 | code/game/cgame/cg_ents.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 47 | code/game/game/g_script.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 45 | code/game/game/q_shared.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 45 | code/game/game/g_mover.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 43 | code/game/botlib/be_ai_char.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 38 | code/game/cgame/cg_consolecmds.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 37 | code/game/cgame/cg_weapons.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 34 | code/game/cgame/cg_local.h:N: warning: 'file' redefined |
| 34 | code/game/game/g_misc.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 31 | code/game/game/g_props.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |
| 31 | code/game/botlib/be_ai_move.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'file' [-Wwrite-strings] |

