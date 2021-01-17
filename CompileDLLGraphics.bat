gcc -O2 -c -o Graphics.o Graphics.c
gcc -O -shared -o Graphics.dll Graphics.o -I.\include -L.\lib -llua53 -lim -lopengl32 -lfreetype

cmd
