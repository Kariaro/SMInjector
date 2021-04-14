#pragma once

typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);
typedef struct luaL_Reg {
	const char *name;
	lua_CFunction func;
} luaL_Reg;