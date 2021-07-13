call vcvarsall.bat x64
cl *.c /Fe: test.exe /DSFMT_MEXP=19937 /F 10485760 /O2 /Os /MT /link gc.lib user32.lib
