#include "stb_vorbis.c"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include "luadef.h"
#include <windows.h>

#include <AL/al.h>
#include <AL/alc.h>




enum Audio_Limits{
    numbuffers = 64,
    numsources = 64,
    givenbuffers = 4
};

drwav wav;

ALCdevice *out; ALCcontext *cont; 
ALuint buffers[numbuffers];
ALuint sources[numsources];
ALuint lastbuff = 0,/*last free buffer */ lastsrc = 0; // last free source




void alPrintError(int line, const char* file){
    ALenum err = alGetError();
    switch(err){
        case AL_INVALID_ENUM:{
            printf("AL_INVALID_ENUM on line %d in %s\n", line, file);
            break;
        }
        case AL_INVALID_VALUE:{
            printf("AL_INVALID_ENUM on line %d in %s\n", line, file);
            break;
        }
        case AL_INVALID_NAME:{
            printf("AL_INVALID_ENUM on line %d in %s\n", line, file);
            break;
        }
        case AL_INVALID_OPERATION:{
            printf("AL_INVALID_ENUM on line %d in %s\n", line, file);
            break;
        }
        case 0:{
            printf("NO ERROR on line %d in %s\n", line, file);
            break;
        }
    }
}

int LU_CheckFileExt(const char* name, const char* ext){
    char *c = (char*)name;
    char *d;
    
    do{
        if (*c == '.') d = c;
    }while(*c++);
    
    for (; *ext; ext++, d++){
        if (*ext != *d) return 0;
        if (*d == '\0' || *ext == '\0') return 0; 
    }
    return 1;
}

typedef struct _SoundCore{
    ALuint src, *buf;
    size_t part;
    ALenum format;
    ALsizei freq;
    void* data;
}SoundCore;



int LA_GetOpenALdata(SoundCore* sc, const char *filename){
    
    if (LU_CheckFileExt(filename, ".wav")){
        if (!drwav_init_file(&wav, filename, NULL)) {
            return -1;
        }
        
        ALenum format;
        if(wav.channels == 1 && wav.bitsPerSample == 8)
            format = AL_FORMAT_MONO8;
        else if(wav.channels == 1 && wav.bitsPerSample == 16)
            format = AL_FORMAT_MONO16;
        else if(wav.channels == 2 && wav.bitsPerSample == 8)
            format = AL_FORMAT_STEREO8;
        else if(wav.channels == 2 && wav.bitsPerSample == 16)
            format = AL_FORMAT_STEREO16;

        size_t part = (size_t)wav.dataChunkDataSize;
        
        char* pSampleData = (char*)malloc(part);
        drwav_read_raw(&wav, part, pSampleData);
        sc->part = part;
        sc->data = pSampleData;
        sc->format = format;
        sc->freq = wav.sampleRate;
        return 0;
    }
    if (LU_CheckFileExt(filename, ".ogg")){
        int channels, sample_rate;
        short *data;
        int len = stb_vorbis_decode_filename(filename, &channels, &sample_rate, &data);
        
        len *= channels*2;
        ALenum format;
        
        switch (channels){
            case 1:
                format = AL_FORMAT_MONO16;
            break;
            case 2:
                format = AL_FORMAT_STEREO16;
            break;
        }
        sc->part = len;
        sc->data = data;
        sc->format = format;
        sc->freq = sample_rate;
        return 0;
    }
    return -3;
}


int Sound_GC(lua_L){
    SoundCore* sc = lua_touserdata(L, 1);
    free(sc->data);
    int type;
    alGetSourcei(sc->src, AL_SOURCE_TYPE, &type);
    // if (type == AL_STREAMING){
    //     for  (int i = bstart+givenbuffers; i < lastbuff; i++){
    //         int a = buffers[i];
            
    //     }
    // }
    return 0;
}

lua_Table smeta[] = {
    {"__gc", &Sound_GC},
    lua_eoT
};



int source_state, source_type, buffers_processed = 0, total_buff_proc = 0;
int cBuffer = 0;
int LE_Load(lua_L){
    
    ALboolean stream = lua_toboolean(L, 3);
    lua_settop(L,2);
    ALuint *topsrc = sources + lastsrc++;
    alGenSources(1, topsrc);

    SoundCore * sc = lua_newuserdata(L, sizeof(SoundCore));
    lua_getglobal(L, "Sound");
    lua_getvalue(L, -1, "gc");
    lua_setmetatable(L, -3);
    lua_pop(L,1);
    lua_setfield(L, 1, "_core");
    lua_getvalue(L, 1, "pitch");
    alSourcef(*topsrc, AL_PITCH, lua_tonumber(L,-1));
    
    lua_getvalue(L, 1, "gain");
    alSourcef(*topsrc, AL_GAIN, lua_tonumber(L,-1));
    
    lua_getvalue(L, 1, "pos");
    lua_getA3(L);

    alSource3f(*topsrc, AL_POSITION, lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
    lua_pop(L,6);
    
    if (!lua_getfield(L, 1, "max_dist")) lua_pushnumber(L, 1500);
    alSourcef(*topsrc, AL_MAX_DISTANCE, lua_tonumber(L,-1));
    
    if (!lua_getfield(L, 1, "rolloff")) lua_pushnumber(L, 1);
    alSourcef(*topsrc, AL_ROLLOFF_FACTOR, lua_tonumber(L,-1));
    
    lua_settop(L, 2);
    
    lua_getvalue(L, 1, "looping");
    alSourcei(*topsrc, AL_LOOPING, lua_toboolean(L, -1));
    
    LA_GetOpenALdata(sc, lua_tostring(L,2));
    
    ALuint *topbuff;
    if (stream){
        topbuff = buffers + lastbuff;
        
        lastbuff += givenbuffers;
        size_t part = sc->part/givenbuffers;
        char* pdata = sc->data;
        alGenBuffers(givenbuffers, topbuff);
        for (int i = 0; i < givenbuffers; i++, pdata+=part){

            alBufferData(topbuff[i], AL_FORMAT_MONO16, pdata, part, sc->freq);
            alSourceQueueBuffers(*topsrc, 1, topbuff+i);
        }
        sc->src = *topsrc;
        sc->buf = topbuff;
        return 0;
    }
    topbuff = buffers + lastbuff++;
    alGenBuffers(1, topbuff);
    
    
    alBufferData(*topbuff, AL_FORMAT_MONO16, sc->data, sc->part, sc->freq);

    alSourcei(*topsrc, AL_BUFFER, *topbuff);
    
    sc->src = *topsrc;
    sc->buf = topbuff;
    return LUA_TNIL;
}




lua_Table func[] = {
    
    {"LoadSound", &LE_Load},

    lua_eoT
};


/* Audio meta for garbage colection  */
int Audio_GC(lua_L){
    //lua_getvalue(L, 1, "source");
    drwav_uninit(&wav);
    alcDestroyContext(cont);
    alcCloseDevice(out);
    alDeleteBuffers(lastbuff, buffers);
    return 0;
}


lua_Table meta[] = {
    {"__gc", &Audio_GC},
    lua_eoT
};

int LE_ListenerUpd(lua_L){
    lua_getvalue(L, 1, "pos");
    lua_geti(L, 2, 1);
    lua_geti(L, 2, 2);
    lua_geti(L, 2, 3);
    
    alListener3f(AL_POSITION, lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));

    return 0;
}

////                                SOURCE-FUNCTIONS                               \\\\\\

int S_Update(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    
    lua_getvalue(L, 1, "pos");
    lua_geti(L, -1, 1);
    lua_geti(L, -2, 2);
    lua_geti(L, -3, 3);

    alSource3f(sc->src, AL_POSITION, lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
    lua_pop(L,3);

    lua_getvalue(L, 1, "vel");
    lua_geti(L, -1, 1);
    lua_geti(L, -2, 2);
    lua_geti(L, -3, 3);

    alSource3f(sc->src, AL_VELOCITY, lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
    lua_pop(L,3);
    
    
    alGetSourcei(sc->src, AL_SOURCE_STATE, &source_state);
    int part = sc->part;
    
    
    if (source_state == AL_PLAYING) {
        // check for errors
        alGetSourcei(sc->src, AL_SOURCE_TYPE, &source_type);
        if (source_type == AL_STREAMING){
            alGetSourcei(sc->src, AL_BUFFERS_PROCESSED, &buffers_processed);
            if (buffers_processed != total_buff_proc){
                total_buff_proc = buffers_processed;
                alSourceUnqueueBuffers(sc->src, 1, &cBuffer);
                part = drwav_read_raw(&wav, part, sc->data);
                alBufferData(cBuffer, sc->format, sc->data, part, (ALsizei)wav.sampleRate);
                alSourceQueueBuffers(sc->src, 1, &cBuffer);
            }
        }
        lua_pushboolean(L, 1);
        return 1;
    }
    
    return 0;
}

int S_Play(lua_L){
    lua_getvalue(L, 1, "_core");
    
    SoundCore * sc = lua_touserdata(L, -1);
    
    alSourcePlay(sc->src);
    
    return LUA_TNIL;
}



int S_setPitch(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    alSourcef(sc->src, AL_PITCH, lua_tonumber(L,2));
    return LUA_TNIL;
}

int S_setGain(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    alSourcef(sc->src, AL_GAIN, lua_tonumber(L,2));
    return LUA_TNIL;
}


int S_setLooping(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    alSourcei(sc->src, AL_LOOPING, lua_toboolean(L, 2));
    return LUA_TNIL;
}

int S_getLooping(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    ALint ret;
    alGetSourcei(sc->src, AL_LOOPING, &ret);
    lua_pushboolean(L, ret);
    return 1;
}

int S_isStream(lua_L){
    lua_getvalue(L, 1, "_core");
    SoundCore *sc = lua_touserdata(L, -1);
    ALint ret;
    alGetSourcei(sc->src, AL_SOURCE_TYPE, &ret);
    lua_pushboolean(L, ret==AL_STREAMING);
    return 1;
}

int S_newSound(lua_L);

lua_Table Sound[] = {
    {"new", &S_newSound},
    {"setGain", &S_setGain},
    {"setLooping", &S_setLooping},
    {"setPitch", &S_setPitch},
    {"isStream", &S_isStream},
    {"Update", &S_Update},
    {"Play", &S_Play},
    lua_eoT
};

int LU_setPos(lua_L, lua_Number a[3]){
    lua_createtable(L, 3, 3);
    
    lua_pushnumber(L, a[0]);
    lua_seti(L, -2, 1);
    lua_pushnumber(L, a[1]);
    lua_seti(L, -2, 2);
    lua_pushnumber(L, a[2]);
    lua_seti(L, -2, 3);
    
    return 1;
}

int S_newSound(lua_L){
    lua_Number a[3] = {0.0, 0.0, 0.0};
    (lua_istable(L, 1))? luaL_setfuncs(L, Sound, 0) : luaL_newlib(L, Sound);
    
    if(!lua_getfield(L, 1, "pos")){
        LU_setPos(L, a); 
        lua_setfield(L, 1, "pos");
    } lua_pop(L,1);

    if (!lua_getfield(L, 1, "vel")) {
        LU_setPos(L, a); 
        lua_setfield(L, 1, "vel");
    } lua_pop(L,1);
    
    if (!lua_getfield(L, 1, "gain")) {
        lua_pushnumber(L, 1); 
        lua_setfield(L, 1, "gain");
    } lua_pop(L,1);
    
    if (!lua_getfield(L, 1, "pitch")){
        lua_pushnumber(L, 1);
        lua_setfield(L, 1, "pitch");
    } lua_pop(L,1);
    
    if (!lua_getfield(L, 1, "looping")) {
        lua_pushboolean(L, 0);
        lua_setfield(L, 1, "looping");
    } lua_pop(L,1);

    
    return 1;
}


int LUA_DLL luaopen_Audio(lua_L)
{
    out = alcOpenDevice(NULL);
    cont = alcCreateContext(out, NULL);
    const ALfloat LisOri[] = {0,0,-1, 0,1,0};
    alcMakeContextCurrent(cont);
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, LisOri);
    
    alDistanceModel(AL_INVERSE_DISTANCE);
    luaL_newlib(L, Sound);
    lua_setglobal(L, "Sound");
    lua_getglobal(L, "Sound");

    luaL_newlib(L, smeta);
    lua_setfield(L, -2, "gc");
    
    luaL_newlib(L, func);  // Audio lib table
    lua_createtable(L, 1, 1);   
    luaL_setfuncs(L, meta, 0);  // Audio lib GC meta
    
    lua_setmetatable(L, -2); // setmetatable(Audio, AudioGC)

    
    lua_createtable(L, 2, 2); // Listener = {}

    lua_createtable(L, 3,3); 
    lua_pushnumber(L, 0.0);
    lua_seti(L, -2, 1);
    lua_pushnumber(L, 0.0);
    lua_seti(L, -2, 2);
    lua_pushnumber(L, 0.0);
    lua_seti(L, -2, 3);      

    lua_setfield(L, -2, "pos"); // Listener.pos = {0,0,0} 

    lua_pushcfunction(L, LE_ListenerUpd);
    lua_setfield(L, -2, "Update");
    
    lua_setfield(L, -2 , "Listener"); // Audio.Listener = Listener


    return 1;
}


