/*
** $Id: lmathlib.c,v 1.67.1.1 2007/12/27 13:02:25 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lrotable.h"


#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)



static int math_abs (lua_State *L) {
  lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sin (lua_State *L) {
  lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
  return 1;
}

static int math_sinh (lua_State *L) {
  lua_pushnumber(L, sinh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cos (lua_State *L) {
  lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
  return 1;
}

static int math_cosh (lua_State *L) {
  lua_pushnumber(L, cosh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tan (lua_State *L) {
  lua_pushnumber(L, tan(luaL_checknumber(L, 1)));
  return 1;
}

static int math_tanh (lua_State *L) {
  lua_pushnumber(L, tanh(luaL_checknumber(L, 1)));
  return 1;
}

static int math_asin (lua_State *L) {
  lua_pushnumber(L, asin(luaL_checknumber(L, 1)));
  return 1;
}

static int math_acos (lua_State *L) {
  lua_pushnumber(L, acos(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan (lua_State *L) {
  lua_pushnumber(L, atan(luaL_checknumber(L, 1)));
  return 1;
}

static int math_atan2 (lua_State *L) {
  lua_pushnumber(L, atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_ceil (lua_State *L) {
  lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
  return 1;
}

static int math_floor (lua_State *L) {
  lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
  return 1;
}

static int math_fmod (lua_State *L) {
  lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_modf (lua_State *L) {
  double ip;
  double fp = modf(luaL_checknumber(L, 1), &ip);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, fp);
  return 2;
}

static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
  return 1;
}

static int math_pow (lua_State *L) {
  lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

static int math_log (lua_State *L) {
  lua_pushnumber(L, log(luaL_checknumber(L, 1)));
  return 1;
}

static int math_log10 (lua_State *L) {
  lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
  return 1;
}

static int math_exp (lua_State *L) {
  lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
  return 1;
}

static int math_deg (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
  return 1;
}

static int math_rad (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
  return 1;
}

static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

static int math_ldexp (lua_State *L) {
  lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
  return 1;
}



static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}


static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}


static int math_random (lua_State *L) {
  /* the `%' avoids the (rare) case of r==1, and is needed also because on
     some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
  lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      int u = luaL_checkint(L, 1);
      luaL_argcheck(L, 1<=u, 1, "interval is empty");
      lua_pushnumber(L, floor(r*u)+1);  /* int between 1 and `u' */
      break;
    }
    case 2: {  /* lower and upper limits */
      int l = luaL_checkint(L, 1);
      int u = luaL_checkint(L, 2);
      luaL_argcheck(L, l<=u, 2, "interval is empty");
      lua_pushnumber(L, floor(r*(u-l+1))+l);  /* int between `l' and `u' */
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  return 1;
}


static int math_randomseed (lua_State *L) {
  srand(luaL_checkint(L, 1));
  return 0;
}


static const luaR_entry math_map[] = {
  {LRO_STRKEY("abs"),   LRO_FUNCVAL(math_abs)},
  {LRO_STRKEY("acos"),  LRO_FUNCVAL(math_acos)},
  {LRO_STRKEY("asin"),  LRO_FUNCVAL(math_asin)},
  {LRO_STRKEY("atan2"), LRO_FUNCVAL(math_atan2)},
  {LRO_STRKEY("atan"),  LRO_FUNCVAL(math_atan)},
  {LRO_STRKEY("ceil"),  LRO_FUNCVAL(math_ceil)},
  {LRO_STRKEY("cosh"),  LRO_FUNCVAL(math_cosh)},
  {LRO_STRKEY("cos"),   LRO_FUNCVAL(math_cos)},
  {LRO_STRKEY("deg"),   LRO_FUNCVAL(math_deg)},
  {LRO_STRKEY("exp"),   LRO_FUNCVAL(math_exp)},
  {LRO_STRKEY("floor"), LRO_FUNCVAL(math_floor)},
  {LRO_STRKEY("fmod"),  LRO_FUNCVAL(math_fmod)},
#if defined(LUA_COMPAT_MOD)
  {LRO_STRKEY("mod"),   LRO_FUNCVAL(math_fmod)},
#endif
  {LRO_STRKEY("frexp"), LRO_FUNCVAL(math_frexp)},
  {LRO_STRKEY("ldexp"), LRO_FUNCVAL(math_ldexp)},
  {LRO_STRKEY("log10"), LRO_FUNCVAL(math_log10)},
  {LRO_STRKEY("log"),   LRO_FUNCVAL(math_log)},
  {LRO_STRKEY("max"),   LRO_FUNCVAL(math_max)},
  {LRO_STRKEY("min"),   LRO_FUNCVAL(math_min)},
  {LRO_STRKEY("modf"),  LRO_FUNCVAL(math_modf)},
  {LRO_STRKEY("pow"),   LRO_FUNCVAL(math_pow)},
  {LRO_STRKEY("rad"),   LRO_FUNCVAL(math_rad)},
  {LRO_STRKEY("random"),     LRO_FUNCVAL(math_random)},
  {LRO_STRKEY("randomseed"), LRO_FUNCVAL(math_randomseed)},
  {LRO_STRKEY("sinh"),  LRO_FUNCVAL(math_sinh)},
  {LRO_STRKEY("sin"),   LRO_FUNCVAL(math_sin)},
  {LRO_STRKEY("sqrt"),  LRO_FUNCVAL(math_sqrt)},
  {LRO_STRKEY("tanh"),  LRO_FUNCVAL(math_tanh)},
  {LRO_STRKEY("tan"),   LRO_FUNCVAL(math_tan)},
  {LRO_STRKEY("pi"),    LRO_NUMVAL(PI)},
  {LRO_STRKEY("huge"),  LRO_NUMVAL(HUGE_VAL)},
  {LRO_NILKEY, LRO_NILVAL}
};

static const luaR_table math_table = LUA_ROTABLE(math_map);


/*
** Open math library
*/
LUALIB_API int luaopen_math (lua_State *L) {
  luaL_register_ro(L, LUA_MATHLIBNAME, &math_table);
  return 1;
}

