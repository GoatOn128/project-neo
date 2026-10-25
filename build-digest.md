# Build log digest

- Log: `build.log`
- Lines: 424
- Errors: 3
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
snd_emitter.obj : error LNK2019: unresolved external symbol "public: __thiscall idSoundChannel::idSoundChannel(void)" (??0idSoundChannel@@QAE@XZ) referenced in function "public: __thiscall idSoundEmitterLocal::idSoundEmitterLocal(void)" (??0idSoundEmitterLocal@@QAE@XZ)
bin\Doom.exe : fatal error LNK1120: 1 unresolved externals
scons: *** [bin\Doom.exe] Error 1120
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | snd_emitter.obj : error LNKN: unresolved external symbol "public: __thiscall idSoundChannel::idSoundChannel(void)" (??NidSoundChannel@@QAE@XZ) referenced in function "public: __thiscall idSoundEmitterLocal::idSoundEmitterLocal(void)" (??NidSoundEmitterLocal@@QAE@XZ) |
| 1 | bin\Doom.exe : fatal error LNKN: N unresolved externals |
| 1 | scons: *** [bin\Doom.exe] Error N |

