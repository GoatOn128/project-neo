# Build log digest

- Log: `build-linux.log`
- Lines: 353
- Errors: 54
- Warnings: 1

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
cl_ui.cpp:(.text+0x700): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cl_cin.cpp:(.text+0x1ba4): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cl_cin.cpp:(.text+0x2e96): undefined reference to `Com_Memset(void*, int, unsigned long)'
cl_keys.cpp:(.text+0xd51): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cl_main.cpp:(.text+0xd52): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cl_main.cpp:(.text+0xd76): undefined reference to `Com_Memset(void*, int, unsigned long)'
cl_main.cpp:(.text+0x4f95): undefined reference to `Com_Memset(void*, int, unsigned long)'
cm_load.cpp:(.text+0x9b): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_load.cpp:(.text+0x145): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_load.cpp:(.text+0x1679): undefined reference to `Com_Memset(void*, int, unsigned long)'
cm_load.cpp:(.text+0xd0c): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_load.cpp:(.text+0xd84): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_load.cpp:(.text+0xdcb): undefined reference to `Com_Memset(void*, int, unsigned long)'
cm_patch.cpp:(.text+0x240c): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_patch.cpp:(.text+0x24b2): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_patch.cpp:(.text+0x2be5): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_patch.cpp:(.text+0x2c18): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_polylib.cpp:(.text+0xc6): undefined reference to `Com_Memset(void*, int, unsigned long)'
cm_polylib.cpp:(.text+0x29d): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_polylib.cpp:(.text+0x8ee): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_polylib.cpp:(.text+0x1590): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_polylib.cpp:(.text+0x17ea): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: cm_polylib.cpp:(.text+0x18d3): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
cm_test.cpp:(.text+0x98a): undefined reference to `Com_Memset(void*, int, unsigned long)'
cm_trace.cpp:(.text+0x2280): undefined reference to `Com_Memset(void*, int, unsigned long)'
common.cpp:(.text+0x1512): undefined reference to `Com_Memset(void*, int, unsigned long)'
common.cpp:(.text+0x1d05): undefined reference to `Com_Memset(void*, int, unsigned long)'
common.cpp:(.text+0x1e15): undefined reference to `Com_Memset(void*, int, unsigned long)'
common.cpp:(.text+0x3da2): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
files.cpp:(.text+0x2567): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
files.cpp:(.text+0x42ce): undefined reference to `Com_Memset(void*, int, unsigned long)'
files.cpp:(.text+0x4800): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
files.cpp:(.text+0x1b18): undefined reference to `Com_Memset(void*, int, unsigned long)'
huffman.cpp:(.text+0x949): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: huffman.cpp:(.text+0xae4): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
huffman.cpp:(.text+0xb97): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: huffman.cpp:(.text+0xc87): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)'
huffman.cpp:(.text+0xcd8): undefined reference to `Com_Memset(void*, int, unsigned long)'
/usr/bin/x86_64-linux-gnu-ld.bfd: huffman.cpp:(.text+0xceb): undefined reference to `Com_Memset(void*, int, unsigned long)'
md4.cpp:(.text+0xb2b): undefined reference to `Com_Memset(void*, int, unsigned long)'
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 2 | files.cpp:(.text+NxN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cl_ui.cpp:(.text+NxN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cl_cin.cpp:(.text+NxNbaN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cl_cin.cpp:(.text+NxNeN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cl_keys.cpp:(.text+NxdN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cl_main.cpp:(.text+NxdN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cl_main.cpp:(.text+NxdN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cl_main.cpp:(.text+NxNfN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cm_load.cpp:(.text+NxNb): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_load.cpp:(.text+NxN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_load.cpp:(.text+NxN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cm_load.cpp:(.text+NxdNc): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_load.cpp:(.text+NxdN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_load.cpp:(.text+Nxdcb): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cm_patch.cpp:(.text+NxNc): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_patch.cpp:(.text+NxNbN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_patch.cpp:(.text+NxNbeN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_patch.cpp:(.text+NxNcN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_polylib.cpp:(.text+NxcN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cm_polylib.cpp:(.text+NxNd): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_polylib.cpp:(.text+NxNee): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_polylib.cpp:(.text+NxN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_polylib.cpp:(.text+NxNea): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | /usr/bin/xN_N-linux-gnu-ld.bfd: cm_polylib.cpp:(.text+NxNdN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |
| 1 | cm_test.cpp:(.text+NxNa): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | cm_trace.cpp:(.text+NxN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | common.cpp:(.text+NxN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | common.cpp:(.text+NxNdN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | common.cpp:(.text+NxNeN): undefined reference to `Com_Memset(void*, int, unsigned long)' |
| 1 | common.cpp:(.text+NxNdaN): undefined reference to `Com_Memcpy(void*, void const*, unsigned long)' |

## Warnings, first 40

```
code/splines/util_list.h:45:1: warning: template-id not allowed for destructor in C++20 [-Wtemplate-id-cdtor]
```

## Warnings that repeat, top 30

| Count | Warning |
| --- | --- |
| 1 | code/splines/util_list.h:N:N: warning: template-id not allowed for destructor in C++N [-Wtemplate-id-cdtor] |

## Warnings by file

| Count | File |
| --- | --- |
| 1 | code/splines/util_list.h |

