all:LuaEngine.exe

LuaEngine.exe: Graphics.dll Audio.dll Collision.dll
	gcc -O2 -I. -L. Core.c -o LuaEngine.exe  -llua53 -liup -liupgl -lopengl32 

Graphics.dll: Graphics.o
	gcc -O -shared -o Graphics.dll Graphics.o -I. -L. -llua53 -lim -lopengl32 -lfreetype

Audio.dll:
	gcc -O -shared -o Audio.dll Audio.o -I. -L. -llua53 -lopenal32

Collision.dll: Collision.o
	gcc -O -shared -o Collision.dll Collision.o -I. -L. -llua53

Graphics.o: Graphics.c
	gcc -O2 -c -o Graphics.o Graphics.c

Audio.o: Audio.c
	gcc -O2 -c -o Audio.o Audio.c

Collision.o: Collision.c
	gcc -O2 -c -o Collision.c Collision.o