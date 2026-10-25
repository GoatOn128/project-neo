# Build log digest

- Log: `build-linux.log`
- Lines: 304
- Errors: 2
- Warnings: 7

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
collect2: error: ld returned 1 exit status
scons: *** [bin/ref_trin_glx64.so] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | collectN: error: ld returned N exit status |
| 1 | scons: *** [bin/ref_trin_glxN.so] Error N |

## Warnings, first 40

```
code/qcommon/unzip.cpp:364:15: warning: 'number_disk' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/unzip.cpp:366:15: warning: 'number_disk_with_CD' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/unzip.cpp:405:9: warning: 'number_entry_CD' may be used uninitialized [-Wmaybe-uninitialized]
code/qcommon/unzip.cpp:896:20: warning: 'size_filename' may be used uninitialized [-Wmaybe-uninitialized]
code/renderer/tr_model.cpp:355:46: warning: 'best_start_i[2]' may be used uninitialized [-Wmaybe-uninitialized]
/usr/include/x86_64-linux-gnu/bits/string_fortified.h:36:34: warning: 'void* __builtin___memmove_chk(void*, const void*, long unsigned int, long unsigned int)' specified bound 18446744073709551615 exceeds maximum object size 9223372036854775807 [-Wstringop-overflow=]   [in code/game/cgame/fx/genericparser2.cpp]
code/sound/opl/doom_opl.cpp:1423:34: warning: comparison of integer expressions of different signedness: 'int' and 'unsigned int' [-Wsign-compare]
```

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code/qcommon/unzip.cpp:N:N: warning: 'number_disk' may be used uninitialized [-Wmaybe-uninitialized] |
| 1 | code/qcommon/unzip.cpp:N:N: warning: 'number_disk_with_CD' may be used uninitialized [-Wmaybe-uninitialized] |
| 1 | code/qcommon/unzip.cpp:N:N: warning: 'number_entry_CD' may be used uninitialized [-Wmaybe-uninitialized] |
| 1 | code/qcommon/unzip.cpp:N:N: warning: 'size_filename' may be used uninitialized [-Wmaybe-uninitialized] |
| 1 | code/renderer/tr_model.cpp:N:N: warning: 'best_start_i[N]' may be used uninitialized [-Wmaybe-uninitialized] |
| 1 | /usr/include/xN_N-linux-gnu/bits/string_fortified.h:N:N: warning: 'void* __builtin___memmove_chk(void*, const void*, long unsigned int, long unsigned int)' specified bound N exceeds maximum object size N [-Wstringop-overflow=]   [in code/game/cgame/fx/genericparserN.cpp] |
| 1 | code/sound/opl/doom_opl.cpp:N:N: warning: comparison of integer expressions of different signedness: 'int' and 'unsigned int' [-Wsign-compare] |

## Warnings by file

| Count | File |
| --- | --- |
| 4 | code/qcommon/unzip.cpp |
| 1 | code/renderer/tr_model.cpp |
| 1 | /usr/include/x86_64-linux-gnu/bits/string_fortified.h |
| 1 | code/sound/opl/doom_opl.cpp |

