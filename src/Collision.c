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

int CC_MoveAway(lua_L){
    lua_Number ax, ay, ar, bx, by, br;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    ar = lua_tonumber(L, 3);

    bx = lua_tonumber(L, 4);
    by = lua_tonumber(L, 5);
    br = lua_tonumber(L, 6);

    
    lua_Number dx = ax - bx, dy = ay - by;

    lua_Number r = dx*dx + dy*dy, rmax = (br + ar)*(br + ar);
    if (r > rmax) {
        lua_pushnumber(L,0.0);
        lua_pushnumber(L,0.0);

        return 2;
    }
    r = sqrt(r); rmax = sqrt(rmax);
    if (r == 0.0){
        r = rmax;
    }
    
    lua_Number sin = dx/r, cos = dy/r;
    lua_Number dr = rmax - r;
    lua_Number x = cos*dr, y = sin*dr;
    
    lua_pushnumber(L,x);
    lua_pushnumber(L,y);

    return 2;
}

int PC_Collide(lua_State* L){ 
    lua_Number ax, ay, bx, by, br;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    
    
    bx = ax - lua_tonumber(L, 3);
    by = ay - lua_tonumber(L, 4);
    br = lua_tonumber(L, 5);
    

    lua_pushboolean(L, (bx*bx + by*by <= br*br));
    return 1;
};

int PC_MoveAway(lua_L){
    lua_Number ax, ay, bx, by, br;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    
    
    bx = ax - lua_tonumber(L, 3);
    by = ay - lua_tonumber(L, 4);
    br = lua_tonumber(L, 5);
    

    lua_Number r = bx*bx + by*by, rmax = br*br;

    if (r >= rmax) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    r = sqrt(r); rmax = sqrt(r);

    lua_Number sin = bx/r, cos = by/r;
    lua_Number dr = rmax - r;
    lua_Number x = cos*dr, y = sin*dr;
    
    lua_pushnumber(L,x);
    lua_pushnumber(L,y);

    return 2;
}

// Checks whether or not point is in the square
int PS_Collide(lua_State* L){ 
    lua_Number ax, ay, bx, by, bw, bh;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    
    
    bx = lua_tonumber(L, 3);
    by = lua_tonumber(L, 4);
    bw = lua_tonumber(L, 5);
    bh = lua_tonumber(L, 6);

    lua_pushboolean(L, ((ax >= bx )&& (ax <= bx + bw )&&(ay >= by) &&( ay <= by + bh)) );
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

int SS_MoveAway(lua_State *L){ // 
    lua_Number ax, ay, aw, ah;
    lua_Number bx, by, bw, bh;
    lua_Number x, y;

    ax = lua_tonumber(L, 1);
    ay = lua_tonumber(L, 2);
    aw = lua_tonumber(L, 3);
    ah = lua_tonumber(L, 4);
    
    bx = lua_tonumber(L, 5);
    by = lua_tonumber(L, 6);
    bw = lua_tonumber(L, 7);
    bh = lua_tonumber(L, 8);

    if (ax + aw >= bx)
        x = ax + aw + 1;
    if (ax <= bx + bw)
        x = bx + bw - 1;
    if (ay + ah >= by)
        y = ay + ah + 1;
    if (ay <= by + bh)
        y = by + bh - 1;
    
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

static const struct luaL_Reg funcs[] = {
   
    {"CtC", &CC_Collide},
    {"CafC", &CC_MoveAway},
    {"PtC", &PC_Collide},
    {"PafC", &PC_MoveAway},
    {"PtS", &PS_Collide},
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