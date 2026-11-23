/*
** Read-only tables (rotable) for Lua 5.1.
*/

#include <string.h>

#define lrotable_c
#define LUA_CORE

#include "lua.h"
#include "ldebug.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lrotable.h"


int luaR_isrotable (const void *p) {
  const luaR_table *t = cast(const luaR_table *, p);
  if (p == NULL)
    return 0;
  return (t->magic1 == LUA_ROTABLE_MAGIC1 &&
          t->magic2 == LUA_ROTABLE_MAGIC2);
}


const TValue *luaR_findentry (void *data, const char *strkey,
                              luaR_numkey numkey, unsigned *ppos) {
  const luaR_table *t = cast(const luaR_table *, data);
  const luaR_entry *pentry;
  unsigned i = 0;
  if (t == NULL || t->pentries == NULL)
    return NULL;
  pentry = t->pentries;
  while (pentry->key.type != LUA_TNIL) {
    if ((strkey != NULL && pentry->key.type == LUA_TSTRING &&
         strcmp(pentry->key.id.strkey, strkey) == 0) ||
        (strkey == NULL && pentry->key.type == LUA_TNUMBER &&
         pentry->key.id.numkey == numkey)) {
      if (ppos != NULL)
        *ppos = i;
      return &pentry->value;
    }
    i++;
    pentry++;
  }
  return NULL;
}


void *luaR_getmeta (void *data) {
  const TValue *res = luaR_findentry(data, "__metatable", 0, NULL);
  if (res != NULL && ttisrotable(res))
    return rvalue(res);
  return NULL;
}


void luaR_getcstr (char *dest, const TString *src, size_t maxsize) {
  if (src->tsv.len + 1 > maxsize)
    dest[0] = '\0';
  else {
    memcpy(dest, getstr(src), src->tsv.len);
    dest[src->tsv.len] = '\0';
  }
}


static void luaR_next_helper (lua_State *L, const luaR_table *t, unsigned pos,
                              TValue *key, TValue *val) {
  const luaR_entry *pentries = t->pentries;
  setnilvalue(key);
  setnilvalue(val);
  if (pentries[pos].key.type != LUA_TNIL) {
    if (pentries[pos].key.type == LUA_TSTRING) {
      setsvalue(L, key, luaS_newro(L, pentries[pos].key.id.strkey));
    }
    else {
      setnvalue(key, cast_num(pentries[pos].key.id.numkey));
    }
    setobj2s(L, val, &pentries[pos].value);
  }
}


void luaR_next (lua_State *L, void *data, TValue *key, TValue *val) {
  const luaR_table *t = cast(const luaR_table *, data);
  char strkey[LUA_MAX_ROTABLE_NAME + 1];
  const char *pstrkey = NULL;
  luaR_numkey numkey = 0;
  unsigned keypos = 0;

  if (ttisnil(key))
    keypos = 0;
  else {
    if (ttisstring(key)) {
      luaR_getcstr(strkey, rawtsvalue(key), LUA_MAX_ROTABLE_NAME);
      pstrkey = strkey;
    }
    else if (ttisnumber(key))
      numkey = cast(luaR_numkey, nvalue(key));
    else {
      luaG_runerror(L, "invalid key to " LUA_QL("next"));
      return;
    }
    if (luaR_findentry(data, pstrkey, numkey, &keypos) == NULL) {
      luaG_runerror(L, "invalid key to " LUA_QL("next"));
      return;
    }
    keypos++;
  }
  luaR_next_helper(L, t, keypos, key, val);
}
