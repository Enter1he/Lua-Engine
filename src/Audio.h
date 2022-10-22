// Audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "stb_vorbis.h"


#include "dr_wav.h"

#include "luadef.h"
#ifdef WIN32
    #include <windows.h>
#endif
#include <AL/al.h>
#include <AL/alc.h>

int open_Audio(lua_L);

#endif