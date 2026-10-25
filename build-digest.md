# Build log digest

- Log: `build.log`
- Lines: 12978
- Errors: 865
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\sound\../client\keys.h(37,3): error: typedef redefinition with different types ('struct qkey_t' vs 'struct qkey_t')
code\game\cgame\../game/bg_public.h(107,2): error: redefinition of enumerator 'CLDMG_SPIRIT'
code\game\cgame\../game/bg_public.h(108,2): error: redefinition of enumerator 'CLDMG_FLAMETHROWER'
code\game\cgame\../game/bg_public.h(109,2): error: redefinition of enumerator 'CLDMG_TESLA'
code\game\cgame\../game/bg_public.h(110,2): error: redefinition of enumerator 'CLDMG_BOSS1LIGHTNING'
code\game\cgame\../game/bg_public.h(111,2): error: redefinition of enumerator 'CLDMG_DEBRIS'
code\game\cgame\../game/bg_public.h(112,2): error: redefinition of enumerator 'CLDMG_MAX'
code\game\cgame\../game/bg_public.h(113,3): error: typedef redefinition with different types ('enum clientDamage_t' vs 'enum clientDamage_t')
code\game\cgame\../game/bg_public.h(203,2): error: redefinition of enumerator 'GT_FFA'
code\game\cgame\../game/bg_public.h(204,2): error: redefinition of enumerator 'GT_TOURNAMENT'
code\game\cgame\../game/bg_public.h(205,2): error: redefinition of enumerator 'GT_SINGLE_PLAYER'
code\game\cgame\../game/bg_public.h(209,2): error: redefinition of enumerator 'GT_TEAM'
code\game\cgame\../game/bg_public.h(210,2): error: redefinition of enumerator 'GT_CTF'
code\game\cgame\../game/bg_public.h(211,2): error: redefinition of enumerator 'GT_WOLF'
code\game\cgame\../game/bg_public.h(213,2): error: redefinition of enumerator 'GT_MAX_GAME_TYPE'
code\game\cgame\../game/bg_public.h(214,3): error: typedef redefinition with different types ('enum gametype_t' vs 'enum gametype_t')
code\game\cgame\../game/bg_public.h(218,2): error: redefinition of enumerator 'GSKILL_EASY'
code\game\cgame\../game/bg_public.h(219,2): error: redefinition of enumerator 'GSKILL_MEDIUM'
code\game\cgame\../game/bg_public.h(220,2): error: redefinition of enumerator 'GSKILL_HARD'
code\game\cgame\../game/bg_public.h(221,2): error: redefinition of enumerator 'GSKILL_MAX'
code\game\cgame\../game/bg_public.h(222,3): error: typedef redefinition with different types ('enum gameskill_t' vs 'enum gameskill_t')
code\game\cgame\../game/bg_public.h(224,16): error: redefinition of enumerator 'GENDER_MALE'
code\game\cgame\../game/bg_public.h(224,29): error: redefinition of enumerator 'GENDER_FEMALE'
code\game\cgame\../game/bg_public.h(224,44): error: redefinition of enumerator 'GENDER_NEUTER'
code\game\cgame\../game/bg_public.h(224,60): error: typedef redefinition with different types ('enum gender_t' vs 'enum gender_t')
code\game\cgame\../game/bg_public.h(238,2): error: redefinition of enumerator 'PM_NORMAL'
code\game\cgame\../game/bg_public.h(239,2): error: redefinition of enumerator 'PM_NOCLIP'
code\game\cgame\../game/bg_public.h(240,2): error: redefinition of enumerator 'PM_SPECTATOR'
code\game\cgame\../game/bg_public.h(241,2): error: redefinition of enumerator 'PM_DEAD'
code\game\cgame\../game/bg_public.h(242,2): error: redefinition of enumerator 'PM_FREEZE'
code\game\cgame\../game/bg_public.h(243,2): error: redefinition of enumerator 'PM_INTERMISSION'
code\game\cgame\../game/bg_public.h(244,3): error: typedef redefinition with different types ('enum pmtype_t' vs 'enum pmtype_t')
code\game\cgame\../game/bg_public.h(247,2): error: redefinition of enumerator 'WEAPON_READY'
code\game\cgame\../game/bg_public.h(248,2): error: redefinition of enumerator 'WEAPON_RAISING'
code\game\cgame\../game/bg_public.h(249,2): error: redefinition of enumerator 'WEAPON_RAISING_TORELOAD'
code\game\cgame\../game/bg_public.h(250,2): error: redefinition of enumerator 'WEAPON_DROPPING'
code\game\cgame\../game/bg_public.h(251,2): error: redefinition of enumerator 'WEAPON_DROPPING_TORELOAD'
code\game\cgame\../game/bg_public.h(252,2): error: redefinition of enumerator 'WEAPON_READYING'
code\game\cgame\../game/bg_public.h(253,2): error: redefinition of enumerator 'WEAPON_RELAXING'
code\game\cgame\../game/bg_public.h(254,2): error: redefinition of enumerator 'WEAPON_VENOM_REST'
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 20 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_EXTRAN' |
| 16 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_USE_ITEMN' |
| 10 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'LEGS_EXTRAN' |
| 9 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_EXTRAN' |
| 8 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_PAINN' |
| 8 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_STANDN_ALTN' |
| 8 | code\sound\../client/client.h(N,N): error: redefinition of enumerator 'KB_BUTTONSN' |
| 8 | code\sound\../client/client.h(N,N): error: redefinition of enumerator 'KB_WBUTTONSN' |
| 5 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_ATTACKN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'HI_N' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_STEP_N' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_FALL_DMG_N' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_FLAKGUNN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_RAISEN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_ATTACKN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_STANDN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_READYN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_RELAXN' |
| 4 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_RELOADN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'AICHAR_STIMSOLDIERN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'WP_MONSTER_ATTACKN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_DEATHN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'EV_EXERTN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_DEATHN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_DEADN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'BOTH_DEADN_WATER' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_GESTUREN' |
| 3 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'WEAP_RELOADN' |
| 2 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'TORSO_STAND_ALTN' |
| 2 | code\game\cgame\../game/bg_public.h(N,N): error: redefinition of enumerator 'WEAP_IDLEN' |

