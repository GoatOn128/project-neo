# Build log digest

- Log: `build-linux.log`
- Lines: 24581
- Errors: 1088
- Warnings: 6864

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code/client/../sys/win32/win_local.h:35:10: fatal error: windows.h: No such file or directory
scons: *** [build/release/wolf/code/client/cl_main.o] Error 1
code/renderer/tr_local.h:536:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:929:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:1002:33: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:1237:100: error: expected ',' or '...' before 'or' token
code/renderer/tr_local.h:1474:69: error: expected ',' or '...' before 'or' token
code/renderer/tr_local.h:536:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:929:25: error: expected unqualified-id before 'or' token
code/renderer/tr_local.h:1002:33: error: expected unqualified-id before 'or' token
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
code/renderer/tr_local.h:1237:100: error: expected ',' or '...' before 'or' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
code/renderer/tr_animation.cpp:95:41: error: expected unqualified-id before 'or' token
code/renderer/tr_animation.cpp:95:43: error: expected primary-expression before '.' token
code/renderer/tr_local.h:1474:69: error: expected ',' or '...' before 'or' token
code/renderer/tr_backend.cpp:560:59: error: expected unqualified-id before 'or' token
code/renderer/tr_backend.cpp:560:61: error: expected primary-expression before '.' token
code/renderer/tr_backend.cpp:560:59: error: expected unqualified-id before 'or' token
code/renderer/tr_backend.cpp:560:61: error: expected primary-expression before '.' token
code/renderer/tr_backend.cpp:560:59: error: expected unqualified-id before 'or' token
code/renderer/tr_backend.cpp:560:61: error: expected primary-expression before '.' token
code/renderer/tr_backend.cpp:561:59: error: expected unqualified-id before 'or' token
code/renderer/tr_backend.cpp:561:61: error: expected primary-expression before '.' token
code/renderer/tr_backend.cpp:561:59: error: expected unqualified-id before 'or' token
code/renderer/tr_backend.cpp:561:61: error: expected primary-expression before '.' token
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 127 | code/renderer/tr_main.cpp:N:N: error: expected unqualified-id before 'or' token |
| 121 | code/renderer/tr_main.cpp:N:N: error: expected primary-expression before '.' token |
| 93 | code/renderer/tr_local.h:N:N: error: expected unqualified-id before 'or' token |
| 63 | code/renderer/tr_shade_calc.cpp:N:N: error: expected unqualified-id before 'or' token |
| 63 | code/renderer/tr_shade_calc.cpp:N:N: error: expected primary-expression before '.' token |
| 62 | code/renderer/tr_local.h:N:N: error: expected ',' or '...' before 'or' token |
| 54 | code/renderer/tr_surface.cpp:N:N: error: expected unqualified-id before 'or' token |
| 54 | code/renderer/tr_surface.cpp:N:N: error: expected primary-expression before '.' token |
| 41 | code/renderer/tr_main.cpp:N:N: error: expected primary-expression before 'or' token |
| 40 | code/renderer/tr_main.cpp:N:N: error: expected primary-expression before '->' token |
| 19 | code/renderer/tr_backend.cpp:N:N: error: expected unqualified-id before 'or' token |
| 18 | code/renderer/tr_sky.cpp:N:N: error: expected unqualified-id before 'or' token |
| 18 | code/renderer/tr_sky.cpp:N:N: error: expected primary-expression before '.' token |
| 17 | code/renderer/tr_vfx.cpp:N:N: error: expected unqualified-id before 'or' token |
| 17 | code/renderer/tr_vfx.cpp:N:N: error: expected primary-expression before '.' token |
| 13 | code/renderer/tr_backend.cpp:N:N: error: expected primary-expression before '.' token |
| 13 | code/renderer/tr_doom.cpp:N:N: error: expected unqualified-id before 'or' token |
| 13 | code/renderer/tr_doom.cpp:N:N: error: expected primary-expression before '.' token |
| 12 | code/renderer/tr_light.cpp:N:N: error: expected primary-expression before 'or' token |
| 12 | code/renderer/tr_light.cpp:N:N: error: expected primary-expression before '->' token |
| 12 | code/renderer/tr_scene.cpp:N:N: error: expected unqualified-id before 'or' token |
| 12 | code/renderer/tr_scene.cpp:N:N: error: expected primary-expression before '.' token |
| 10 | code/game/cgame/fx/FxTemplate.cpp:N:N: error: 'stricmp' was not declared in this scope; did you mean 'strncmp'? |
| 9 | code/renderer/tr_animation.cpp:N:N: error: expected unqualified-id before 'or' token |
| 9 | code/renderer/tr_animation.cpp:N:N: error: expected primary-expression before '.' token |
| 9 | code/renderer/tr_cmesh.cpp:N:N: error: expected unqualified-id before 'or' token |
| 9 | code/renderer/tr_cmesh.cpp:N:N: error: expected primary-expression before '.' token |
| 9 | code/renderer/tr_mesh.cpp:N:N: error: expected unqualified-id before 'or' token |
| 9 | code/renderer/tr_mesh.cpp:N:N: error: expected primary-expression before '.' token |
| 6 | code/renderer/tr_decals.cpp:N:N: error: expected unqualified-id before 'or' token |

## Errors by file

| Count | File |
| --- | --- |
| 337 | code/renderer/tr_main.cpp |
| 155 | code/renderer/tr_local.h |
| 126 | code/renderer/tr_shade_calc.cpp |
| 108 | code/renderer/tr_surface.cpp |
| 38 | code/renderer/tr_backend.cpp |
| 36 | code/renderer/tr_sky.cpp |
| 34 | code/renderer/tr_vfx.cpp |
| 27 | code/renderer/tr_light.cpp |
| 26 | code/renderer/tr_doom.cpp |
| 24 | code/renderer/tr_scene.cpp |
| 18 | code/renderer/tr_animation.cpp |
| 18 | code/renderer/tr_cmesh.cpp |
| 18 | code/renderer/tr_mesh.cpp |
| 12 | code/renderer/tr_decals.cpp |
| 10 | code/game/cgame/fx/FxTemplate.cpp |
| 8 | code/renderer/tr_flares.cpp |
| 8 | code/renderer/tr_shadows.cpp |
| 7 | code/renderer/tr_glprogs.cpp |
| 6 | code/renderer/tr_shade.cpp |
| 6 | code/renderer/tr_world.cpp |
| 6 | code/game/cgame/fx/FxScheduler.h |
| 6 | code/game/cgame/fx/genericparser2.cpp |
| 5 | code/sys/../renderer/tr_local.h |
| 3 | code/game/cgame/fx/FxScheduler.cpp |
| 1 | code/client/../sys/win32/win_local.h |
| 1 | code/renderer/tr_model.cpp |

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 2643 | code/game/game/bg_misc.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 536 | code/client/cl_keys.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 522 | code/game/ui/ui_shared.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 303 | code/game/cgame/cg_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 223 | code/game/ui/ui_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 220 | code/game/game/bg_animation.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 199 | code/game/game/g_spawn.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 148 | code/qcommon/msg.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 136 | code/game/game/g_main.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 131 | code/game/botlib/be_ai_chat.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 115 | code/game/botlib/l_precomp.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 107 | code/game/cgame/cg_players.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 85 | code/game/botlib/be_aas_reach.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 79 | code/game/botlib/l_script.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 78 | code/game/game/g_combat.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 66 | code/game/cgame/cg_event.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 64 | code/game/botlib/be_ai_weap.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 61 | code/renderer/tr_shader.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 58 | code/game/botlib/be_ai_goal.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 51 | code/game/cgame/cg_ents.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 47 | code/game/game/g_script.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 45 | code/game/game/q_shared.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 45 | code/game/game/g_mover.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 43 | code/game/botlib/be_ai_char.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 38 | code/game/cgame/cg_consolecmds.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 37 | code/game/cgame/cg_weapons.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 34 | code/game/cgame/cg_local.h:N: warning: "CHAR_WIDTH" redefined |
| 34 | code/game/game/g_misc.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 31 | code/game/game/g_props.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |
| 31 | code/game/botlib/be_ai_move.cpp:N:N: warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings] |

## Warnings by file

| Count | File |
| --- | --- |
| 2643 | code/game/game/bg_misc.cpp |
| 536 | code/client/cl_keys.cpp |
| 524 | code/game/ui/ui_shared.cpp |
| 304 | code/game/cgame/cg_main.cpp |
| 224 | code/game/game/bg_animation.cpp |
| 224 | code/game/ui/ui_main.cpp |
| 199 | code/game/game/g_spawn.cpp |
| 151 | code/qcommon/msg.cpp |
| 139 | code/game/game/g_main.cpp |
| 134 | code/game/botlib/be_ai_chat.cpp |
| 118 | code/game/botlib/l_precomp.cpp |
| 108 | code/game/cgame/cg_players.cpp |
| 99 | code/game/botlib/be_aas_reach.cpp |
| 80 | code/game/game/g_combat.cpp |
| 79 | code/game/botlib/l_script.cpp |
| 66 | code/game/cgame/cg_event.cpp |
| 65 | code/game/botlib/be_ai_weap.cpp |
| 61 | code/renderer/tr_shader.cpp |
| 60 | code/game/botlib/be_ai_goal.cpp |
| 55 | code/game/cgame/cg_ents.cpp |
| 50 | code/game/game/q_shared.cpp |
| 47 | code/game/game/g_script.cpp |
| 45 | code/game/game/g_mover.cpp |
| 44 | code/game/botlib/be_ai_char.cpp |
| 40 | code/game/cgame/cg_consolecmds.cpp |
| 38 | code/game/cgame/cg_weapons.cpp |
| 34 | code/game/cgame/cg_local.h |
| 34 | code/game/game/g_misc.cpp |
| 31 | code/game/game/g_props.cpp |
| 31 | code/game/botlib/be_ai_move.cpp |
| 31 | code/game/botlib/be_ai_weight.cpp |
| 26 | code/game/cgame/cg_effects.cpp |
| 24 | code/game/botlib/be_aas_cluster.cpp |
| 23 | code/game/botlib/be_aas_routetable.cpp |
| 21 | code/game/game/g_cmds.cpp |
| 21 | code/game/game/g_save.cpp |
| 21 | code/game/botlib/be_aas_main.cpp |
| 19 | code/game/botlib/be_interface.cpp |
| 18 | code/game/cgame/fx/genericparser2.cpp |
| 16 | code/game/botlib/be_aas_route.cpp |
| 16 | code/game/botlib/be_aas_sample.cpp |
| 15 | code/game/cgame/cg_sound.cpp |
| 15 | code/game/game/g_team.cpp |
| 14 | code/game/botlib/be_aas_main.h |
| 13 | code/game/botlib/l_struct.cpp |
| 12 | code/qcommon/files.cpp |
| 11 | code/client/cl_parse.cpp |
| 11 | code/game/cgame/cg_info.cpp |
| 11 | code/game/game/g_client.cpp |
| 10 | code/game/game/q_math.cpp |
| 10 | code/server/sv_client.cpp |
| 10 | code/game/game/g_missile.cpp |
| 10 | code/game/game/g_script_actions.cpp |
| 10 | code/game/botlib/be_aas_file.cpp |
| 9 | code/game/botlib/be_aas_debug.cpp |
| 9 | code/game/botlib/l_log.cpp |
| 8 | code/qcommon/common.cpp |
| 8 | code/renderer/tr_backend.cpp |
| 8 | code/renderer/tr_image.cpp |
| 8 | code/game/cgame/fx/../cg_local.h |
| 8 | code/game/cgame/fx/genericparser2.h |
| 8 | code/game/game/g_tramcar.cpp |
| 8 | code/game/botlib/be_aas_entity.cpp |
| 7 | code/game/botlib/be_aas_bspq3.cpp |
| 7 | code/sound/opl/doom_opl.cpp |
| 6 | code/game/cgame/cg_particles.cpp |
| 6 | code/game/game/g_doomai.cpp |
| 6 | code/game/ui/ui_players.cpp |
| 6 | code/game/botlib/../game/q_shared.h |
| 6 | code/splines/math_matrix.h |
| 5 | code/server/sv_init.cpp |
| 5 | code/server/sv_main.cpp |
| 5 | code/renderer/tr_bsp.cpp |
| 5 | code/renderer/tr_shade.cpp |
| 4 | code/qcommon/cm_patch.cpp |
| 4 | code/game/game/g_trigger.cpp |
| 3 | code/qcommon/net_chan.cpp |
| 3 | code/server/sv_bot.cpp |
| 3 | code/server/sv_ccmds.cpp |
| 3 | code/game/cgame/../game/q_shared.h |
| 3 | code/game/cgame/cg_doommap.cpp |
| 3 | code/game/game/g_utils.cpp |
| 3 | code/game/game/g_doomweapons.cpp |
| 3 | code/game/ui/ui_gameinfo.cpp |
| 3 | code/splines/q_shared.cpp |
| 2 | code/qcommon/cm_trace.cpp |
| 2 | code/qcommon/cmd.cpp |
| 2 | code/qcommon/cvar.cpp |
| 2 | code/qcommon/unzip.cpp |
| 2 | code/renderer/tr_shade_calc.cpp |
| 2 | code/game/game/g_active.cpp |
| 2 | code/game/game/g_local.h |
| 2 | code/game/game/g_target.cpp |
| 2 | code/game/game/g_doombots_goals.cpp |
| 2 | code/game/botlib/be_aas_move.cpp |
| 2 | /usr/include/x86_64-linux-gnu/bits/string_fortified.h |
| 2 | code/game/botlib/be_ai_gen.cpp |
| 1 | code/qcommon/cm_load.cpp |
| 1 | code/sys/sys_main.cpp |
| 1 | code/renderer/tr_model.cpp |
| 1 | code/renderer/tr_scene.cpp |
| 1 | code/game/cgame/cg_localents.cpp |
| 1 | code/game/cgame/cg_newDraw.cpp |
| 1 | code/game/cgame/cg_playerstate.cpp |
| 1 | code/game/cgame/cg_servercmds.cpp |
| 1 | code/game/cgame/fx/FxScheduler.cpp |
| 1 | code/game/game/g_alarm.cpp |
| 1 | code/game/game/g_weapon.cpp |
| 1 | code/game/game/g_doomitems.cpp |
| 1 | code/game/botlib/be_aas_optimize.cpp |
| 1 | code/game/botlib/l_libvar.cpp |
| 1 | code/game/botlib/l_memory.cpp |

