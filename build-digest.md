# Build log digest

- Log: `build.log`
- Lines: 359
- Errors: 76
- Warnings: 0

**Verdict: the build failed, the errors are below.**

## Errors, first 40

```
code\sound\snd_cache.cpp(27,16): error: cannot define or redeclare 'idSoundSample' here because namespace '' does not enclose namespace 'idSoundSample'
code\sound\snd_cache.cpp(28,2): error: use of undeclared identifier 'name'
code\sound\snd_cache.cpp(29,2): error: use of undeclared identifier 'buffer'
code\sound\snd_cache.cpp(30,2): error: use of undeclared identifier 'rate'
code\sound\snd_cache.cpp(31,2): error: use of undeclared identifier 'channels'
code\sound\snd_cache.cpp(32,2): error: use of undeclared identifier 'numSamples'
code\sound\snd_cache.cpp(33,2): error: use of undeclared identifier 'loaded'
code\sound\snd_cache.cpp(34,2): error: use of undeclared identifier 'isDefault'; did you mean 'axisDefault'?
code\sound\snd_cache.cpp(34,12): error: array type 'vec3_t[3]' (aka 'float[3][3]') is not assignable
code\sound\snd_cache.cpp(35,2): error: use of undeclared identifier 'neverPurge'
code\sound\snd_cache.cpp(36,2): error: use of undeclared identifier 'levelLoadReferenced'
code\sound\snd_cache.cpp(39,20): error: cannot define or redeclare 'LengthInMsec' here because namespace '' does not enclose namespace 'idSoundSample'
code\sound\snd_cache.cpp(40,27): error: use of undeclared identifier 'numSamples'
code\sound\snd_cache.cpp(43,21): error: cannot define or redeclare 'Load' here because namespace '' does not enclose namespace 'idSoundSample'
code\sound\snd_cache.cpp(44,7): error: use of undeclared identifier 'loaded'
code\sound\snd_cache.cpp(47,8): error: use of undeclared identifier 'name'
code\sound\snd_cache.cpp(48,3): error: use of undeclared identifier 'MakeDefault'
code\sound\snd_cache.cpp(52,34): error: use of undeclared identifier 'name'
code\sound\snd_cache.cpp(59,3): error: use of undeclared identifier 'MakeDefault'
code\sound\snd_cache.cpp(66,68): error: use of undeclared identifier 'name'
code\sound\snd_cache.cpp(67,3): error: use of undeclared identifier 'MakeDefault'
code\sound\snd_cache.cpp(76,52): error: use of undeclared identifier 'name'
code\sound\snd_cache.cpp(77,3): error: use of undeclared identifier 'MakeDefault'
code\sound\snd_cache.cpp(80,2): error: use of undeclared identifier 'buffer'
code\sound\snd_cache.cpp(81,2): error: use of undeclared identifier 'rate'
code\sound\snd_cache.cpp(82,2): error: use of undeclared identifier 'channels'
code\sound\snd_cache.cpp(83,2): error: use of undeclared identifier 'numSamples'
code\sound\snd_cache.cpp(84,2): error: use of undeclared identifier 'loaded'
code\sound\snd_cache.cpp(85,2): error: use of undeclared identifier 'isDefault'; did you mean 'axisDefault'?
code\sound\snd_cache.cpp(85,12): error: array type 'vec3_t[3]' (aka 'float[3][3]') is not assignable
code\sound\snd_cache.cpp(89,21): error: cannot define or redeclare 'MakeDefault' here because namespace '' does not enclose namespace 'idSoundSample'
code\sound\snd_cache.cpp(91,2): error: use of undeclared identifier 'FreeData'
code\sound\snd_cache.cpp(100,9): error: redefinition of 'alBuffer'
code\sound\snd_cache.cpp(108,2): error: use of undeclared identifier 'buffer'
code\sound\snd_cache.cpp(109,2): error: use of undeclared identifier 'rate'
code\sound\snd_cache.cpp(110,2): error: use of undeclared identifier 'channels'
code\sound\snd_cache.cpp(111,2): error: use of undeclared identifier 'numSamples'
code\sound\snd_cache.cpp(112,2): error: use of undeclared identifier 'loaded'
code\sound\snd_cache.cpp(113,2): error: use of undeclared identifier 'isDefault'; did you mean 'axisDefault'?
code\sound\snd_cache.cpp(113,12): error: array type 'vec3_t[3]' (aka 'float[3][3]') is not assignable
```

## Errors that repeat, top 30

| Count | Error |
| --- | --- |
| 6 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'buffer' |
| 6 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'count'; did you mean 'std::count'? |
| 5 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'name' |
| 5 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'loaded' |
| 5 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'sampleName' |
| 4 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'numSamples' |
| 4 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'isDefault'; did you mean 'axisDefault'? |
| 4 | code\sound\snd_cache.cpp(N,N): error: array type 'vecN_t[N]' (aka 'float[N][N]') is not assignable |
| 4 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'MakeDefault' |
| 4 | code\sound\snd_cache.cpp(N,N): error: redefinition of 'i' |
| 3 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'rate' |
| 3 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'channels' |
| 2 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'count' |
| 2 | code\sound\snd_cache.cpp(N,N): error: reference to overloaded function could not be resolved; did you mean to call it? |
| 2 | code\sound\snd_cache.cpp(N,N): error: 'name' is a private member of 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'idSoundSample' here because namespace '' does not enclose namespace 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'neverPurge' |
| 1 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'levelLoadReferenced' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'LengthInMsec' here because namespace '' does not enclose namespace 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'Load' here because namespace '' does not enclose namespace 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'MakeDefault' here because namespace '' does not enclose namespace 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: use of undeclared identifier 'FreeData' |
| 1 | code\sound\snd_cache.cpp(N,N): error: redefinition of 'alBuffer' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'FreeData' here because namespace '' does not enclose namespace 'idSoundSample' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'idSoundCache' here because namespace '' does not enclose namespace 'idSoundCache' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'FindOrCreate' here because namespace '' does not enclose namespace 'idSoundCache' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'BeginLevelLoad' here because namespace '' does not enclose namespace 'idSoundCache' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'EndLevelLoad' here because namespace '' does not enclose namespace 'idSoundCache' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'FreeAll' here because namespace '' does not enclose namespace 'idSoundCache' |
| 1 | code\sound\snd_cache.cpp(N,N): error: cannot define or redeclare 'NumLoaded' here because namespace '' does not enclose namespace 'idSoundCache' |

