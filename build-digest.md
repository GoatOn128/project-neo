# Build log digest

- Log: `build.log`
- Lines: 70
- Errors: 4
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\sound\idtech4\snd_system.cpp(188,36): error: use of undeclared identifier 'g_sv'
scons: *** [build\release\wolf\code\sound\idtech4\snd_system.obj] Error 1
code\sound\idtech4\snd_emitter.cpp(98,26): error: 'this' argument to member function 'GetSample' has type 'const idSoundShader', but function is not marked const
scons: *** [build\release\wolf\code\sound\idtech4\snd_emitter.obj] Error 1
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 1 | code\sound\idtechN\snd_system.cpp(N,N): error: use of undeclared identifier 'g_sv' |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_system.obj] Error N |
| 1 | code\sound\idtechN\snd_emitter.cpp(N,N): error: 'this' argument to member function 'GetSample' has type 'const idSoundShader', but function is not marked const |
| 1 | scons: *** [build\release\wolf\code\sound\idtechN\snd_emitter.obj] Error N |

