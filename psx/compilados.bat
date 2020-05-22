set prog=main
set address=$80100000

erase %prog%.exe

msdos ccpsx -O3 -Xo%address% %prog%.c -o%prog%.cpe,,%prog%.map
msdos cpe2x /CE %prog%.cpe
rem send main.exe COM24