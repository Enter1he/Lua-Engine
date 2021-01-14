gcc -O2 -c -o Graphics.o Graphics.c
gcc -O -shared -o Graphics.dll Graphics.o -IC:\LifeHack\CCore\include -LC:\LifeHack\CCore\lib -llua53 -lim -lopengl32 -lfreetype

cmd