gcc -O2 -c -o Audio.o Audio.c
gcc -O -shared -o Audio.dll Audio.o -I. -L. -llua53 -lopenal32
