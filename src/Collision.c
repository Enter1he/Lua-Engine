#include "Collision.h"

// C - means circle, P - point, S - square

int CC_Collide(lua_State *L){ 
    lua_Number ax, ay, ar, bx, by, br;
    

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    ar = lua_tonumber(L, 3);

    bx = lua_tonumber(L, 4);
    by = lua_tonumber(L, 5);
    br = lua_tonumber(L, 6);

    lua_pushboolean(L, ( ((ax - bx)*(ax - bx) + (ay - by)*(ay - by)) <= (br + ar)*(br + ar) ) );
    return 1;
}

int PS_Collide(lua_State* L){ 
    lua_Number ax, ay, bx, by, bw, bh;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    
    
    bx = lua_tonumber(L, 5);
    by = lua_tonumber(L, 6);
    bw = lua_tonumber(L, 7);
    bh = lua_tonumber(L, 8);

    lua_pushboolean(L, ((ax >= bx )&& (ax <= bx + bw )&&(ay >= by) &&( ay <= by + bh)) );
    return 1;
};

int PC_Collide(lua_State* L){ 
    lua_Number ax, ay, bx, by, br;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    
    
    bx = ax - lua_tonumber(L, 5);
    by = ay - lua_tonumber(L, 6);
    br = lua_tonumber(L, 7);
    

    lua_pushboolean(L, (bx*bx + by*by <= br*br));
    return 1;
};

int SS_Collide(lua_State *L){ // 
    lua_Number ax, ay, aw, ah;
    lua_Number bx, by, bw, bh;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    aw = lua_tonumber(L, 3);
    ah = lua_tonumber(L, 4);
    
    bx = lua_tonumber(L, 5);
    by = lua_tonumber(L, 6);
    bw = lua_tonumber(L, 7);
    bh = lua_tonumber(L, 8);


    lua_pushboolean(L, ((ax + aw >= bx )&&(ax <= bx + bw) && (ay + ah >= by)&&(ay <= by + bh)));

    return 1;
}

static const struct luaL_Reg funcs[] = {
   
    {"CtC", &CC_Collide},
    {"PtS", &PS_Collide},
    {"PtC", &PC_Collide},
    {"StS", &SS_Collide},
    {NULL, NULL}
};

LUA_DLL int luaopen_Collision(lua_State* L){
    luaL_newlib(L, funcs);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "Collision");
    lua_nameAtable(L, -2, "Collision");

    return 0;
};