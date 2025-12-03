BIT = -m64
LIB = ./lib64/
LUALIB = ./lua-internals/src/liblua.a
BUILD = ./Win64/
CC = gcc
AR = ar
CORE = src/Core.c
LINK = -liup -liupgl -lopengl32 -lglew32 -lfreetype -lopenal32
RESULT = LuaEngine.exe

ifeq ($(win32), 1)
	BIT = -m32
	LIB = ./lib32/
	BUILD = ./Win32/
	LINK = -liup -liupgl -lopengl32 -lglew32 -lfreetype -lopenal32
	CORE = src/Core.c
	RESULT = LuaEngine.exe
else ifeq ($(emscripten), 1)
	BIT = 
	LIB = ./emlib/
	BUILD = ./embuild/
	CORE = src/emscripten/Core.c src/luadef.c
	CC = emcc 
	AR = emar
	COPT = -fsanitize=undefined -s SAFE_HEAP_LOG=1 -s ASSERTIONS=2 -s USE_WEBGL2 -s LLD_REPORT_UNDEFINED -s WASM=3 -s USE_FREETYPE=3 --preload-file ./embuild/@ --use-preload-plugins -s ALLOW_MEMORY_GROWTH=1 
	RESULT = LuaEngine.js
	LINK = 
endif



modules = $(BUILD)modules

all: $(BUILD)$(RESULT) $(BUILD)modules $(BUILD)modules/% $(BUILD)classes $(BUILD)classes/% $(BUILD)Scenes $(BUILD)Scenes/%

cleanGraph:
	del $(BUILD)Graphics.dll && del $(BUILD)Graphics.o

cleanAudio:
	del $(BUILD)Audio.o && del $(BUILD)Audio.dll

cleanExe:
	del "$(BUILD)LuaEngine.exe"

cleanCollision:
	del $(BUILD)Collision.o && del $(BUILD)Collision.dll 

cleanAll:
	del $(BUILD)LuaEngine.exe && del $(BUILD)Graphics.dll && del $(BUILD)Graphics.o && del $(BUILD)Audio.o && del $(BUILD)Audio.dll && del $(BUILD)Collision.o && del $(BUILD)Collision.dll 


$(BUILD)classes:
	mkdir "$(BUILD)classes"

$(BUILD)classes/%:
	xcopy /e /y /c "./classes" "$(BUILD)classes"
	
$(BUILD)modules:
	mkdir "$(BUILD)modules/%"

$(BUILD)modules/%:
	xcopy /e /y /c "./modules" "$(BUILD)modules"
	
$(BUILD)Scenes:
	mkdir "$(BUILD)Scenes"

$(BUILD)Scenes/%:
	xcopy /e /Y "./Scenes" "$(BUILD)Scenes"
	copy "config.lua" "$(BUILD)config.lua"
	


$(BUILD)$(RESULT): $(CORE) src/LEGlobals.h src/luadef.h $(BUILD)Graphics.o $(BUILD)Audio.o $(BUILD)Collision.o
	$(CC) $(BIT) -I. -L$(LIB) $(CORE) $(BUILD)Graphics.o $(BUILD)Audio.o $(BUILD)Collision.o src/Graphics/Text.c src/Graphics/Sprite.c src/Graphics/Batch.c src/Graphics/mat4.c -o $(BUILD)$(RESULT) $(LUALIB) \
	$(LINK) $(COPT)

Opt: $(BUILD)Graphics.dll $(BUILD)Audio.dll $(BUILD)Collision.dll
	$(CC) $(BIT) -O3 -I. -L$(LIB) src/Core.c -o $(BUILD)LuaEngineOpt.exe -llua54 -liup -liupgl -lopengl32

# $(BUILD)libGraphics.a: $(BUILD)Graphics.o 
# 	$(AR) rcu $(BUILD)libGraphics.a $(BUILD)Graphics.o  $(LIB)liblua.a 

# $(BUILD)libAudio.a: $(BUILD)Audio.o
# 	$(AR) rcu $(BUILD)libAudio.a $(BUILD)Audio.o

$(BUILD)Collision.dll: $(BUILD)Collision.o
	$(CC) $(BIT) -O2 -shared -o $(BUILD)Collision.dll $(BUILD)Collision.o -I. -L$(LIB) -llua54

$(BUILD)Graphics.o: src/Graphics/Graphics.c src/Graphics/Text.c src/Graphics/Sprite.c src/Graphics/Batch.c src/Graphics/mat4.c 
	$(CC) $(BIT) -I.  -c -o $(BUILD)Graphics.o src/Graphics/Graphics.c $(LINK) $(COPT)
 
$(BUILD)Audio.o: src/Audio.c 
	$(CC) $(BIT) -I.  -c -o $(BUILD)Audio.o src/Audio.c

$(BUILD)Collision.o: src/Collision.c
	$(CC) $(BIT) -I.  -c src/Collision.c  -o $(BUILD)Collision.o

