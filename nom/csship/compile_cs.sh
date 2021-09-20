call vsvars32
csc /out:test.exe /target:exe *.cs
timeout 3
editbin /STACK:50000000 test.exe