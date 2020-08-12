# Lua-Engine
Pure Lua Engine

IUP 3.29 for lua 5.3 required for interpretation


Building advice 

Download IUP 3.29 for lua 5.3.5 you'll need srlua from LuaBinaries

IUP https://sourceforge.net/projects/iup/files/3.29/Tools%20Executables/Lua53/

srlua https://joedf.ahkscript.org/LuaBuilds/

Custom libs in /libs source code: https://github.com/Enter1he/LuaEngine---DLLs

Source of release executable: https://github.com/Enter1he/Lua-Engine-Release-Exe

You can build an executable by placing srlua and glue in IUP folder and then
using this command in CMD:

cd *Your repository*
glue srlua.exe main.lua LuaEngine-player.exe

Don't forget to add IUP dlls to /libs folder!

Good luck!
