BIT = -m64
LIB = ./lib64/
BUILD = ./Win64/

ifeq ($(win32), 1)
	BIT = -m32
	LIB = ./lib32/
	BUILD = ./Win32/
endif

all: $(BUILD)LuaEngine.exe $(BUILD)modules $(BUILD)modules/% $(BUILD)classes $(BUILD)classes/% $(BUILD)Scenes $(BUILD)Scenes/%

cleanGraph:
	del $(BUILD)Graphics.dll && del $(BUILD)Graphics.o

cleanAudio:
	del $(BUILD)Audio.o && del $(BUILD)Audio.dll

cleanExe:
	del $(BUILD)LuaEngine.exe

cleanCollision:
	del $(BUILD)Collision.o && del $(BUILD)Collision.dll 

cleanAll:
	del $(BUILD)LuaEngine.exe && del $(BUILD)Graphics.dll && del $(BUILD)Graphics.o && del $(BUILD)Audio.o && del $(BUILD)Audio.dll && del $(BUILD)Collision.o && del $(BUILD)Collision.dll 


$(BUILD)classes:
	mkdir "$(BUILD)classes"

$(BUILD)classes/%:
	xcopy /e /y "./classes" "$(BUILD)classes"
	
$(BUILD)modules:
	mkdir "$(BUILD)modules/%"

$(BUILD)modules/%:
	xcopy /e /y "./modules" "$(BUILD)modules"
	
$(BUILD)Scenes:
	mkdir "$(BUILD)Scenes"

$(BUILD)Scenes/%:
	xcopy /e /y "./Scenes" "$(BUILD)Scenes"
	


$(BUILD)LuaEngine.exe: src/Core.c $(BUILD)Graphics.dll $(BUILD)Audio.dll $(BUILD)Collision.dll
	gcc $(BIT) -I. -L$(LIB) src/Core.c -o $(BUILD)LuaEngine.exe -llua54 -liup -liupgl -lopengl32

Opt: $(BUILD)Graphics.dll $(BUILD)Audio.dll $(BUILD)Collision.dll
	gcc $(BIT) -O3 -I. -L$(LIB) src/Core.c -o $(BUILD)LuaEngineOpt.exe -llua54 -liup -liupgl -lopengl32

$(BUILD)Graphics.dll: $(BUILD)Graphics.o
	gcc $(BIT) -O2 -shared -o $(BUILD)Graphics.dll $(BUILD)Graphics.o -I. -L$(LIB) -lopengl32 -lglew32 -lfreetype  -llua54 -lim -lim_process

$(BUILD)Audio.dll: $(BUILD)Audio.o
	gcc $(BIT) -O2 -shared -o $(BUILD)Audio.dll $(BUILD)Audio.o src/dr_wav.c src/stb_vorbis.c -I. -L$(LIB) -llua54 -lopenal32

$(BUILD)Collision.dll: $(BUILD)Collision.o
	gcc $(BIT) -O2 -shared -o $(BUILD)Collision.dll $(BUILD)Collision.o -I. -L$(LIB) -llua54

$(BUILD)Graphics.o: src/Graphics.c
	gcc $(BIT) -O2 -c -o $(BUILD)Graphics.o src/Graphics.c

$(BUILD)Audio.o: src/Audio.c
	gcc $(BIT) -O2 -c -o $(BUILD)Audio.o src/Audio.c 

$(BUILD)Collision.o: src/Collision.c
	gcc $(BIT) -O2 -c Collision.c  -o $(BUILD)Collision.o

