# Build log digest

- Log: `build.log`
- Lines: 85
- Errors: 8
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\sound\idtech4\sound.h(19,10): fatal error: '../../qcommon/q_shared.h' file not found
scons: *** [build\release\wolf\code\sound\idtech4\snd_system.obj] Error 1
code\sound\idtech4\sound.h(19,10): fatal error: '../../qcommon/q_shared.h' file not found
scons: *** [build\release\wolf\code\sound\idtech4\snd_world.obj] Error 1
code\sound\idtech4\sound.h(19,10): fatal error: '../../qcommon/q_shared.h' file not found
scons: *** [build\release\wolf\code\sound\idtech4\snd_emitter.obj] Error 1
code\sound\idtech4\sound.h(19,10): fatal error: '../../qcommon/q_shared.h' file not found
scons: *** [build\release\wolf\code\sound\idtech4\snd_shader.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 4 | code\sound\idtechN\sound.h(N,N): fatal error: '../../qcommon/q_shared.h' file not found |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_system.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_world.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_emitter.obj] Error N |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_shader.obj] Error N |

