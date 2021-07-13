call vcvarsall.bat x86
cl *.c /Fe: testdbg.exe /DSFMT_MEXP=19937 /Zi /DEBUG  /F 10485760 /MTd /link gc_debugx86.lib user32.lib
