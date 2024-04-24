#ifndef SPRITE_H
#define SPRITE_H

#include "src/Graphics/Graphics.h"

#include "include/im.h"
#include "include/im_image.h"
#include "include/im_convert.h"
#include "include/im_process.h"

int LE_LoadSingleSprite(lua_L);
int LE_LoadSpriteSheet(lua_L);

int LE_DrawSingleSprite(lua_L);
int LE_DrawSpriteSheet(lua_L);

int Sp_GetOSize(lua_L);
int Sp_ResetOSize(lua_L);
int Sp_SetOSize(lua_L);

#endif