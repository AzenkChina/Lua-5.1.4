/*
** Read-only tables (rotable) for Lua 5.1.
** A rotable is a const array of entries in Flash/ROM; it is not a GC
** object and its keys/values are never copied into the Lua heap.
*/

#ifndef lrotable_h
#define lrotable_h

#include "lua.h"
#include "lobject.h"


/* maximum length of a rotable name and of a string key */
#define LUA_MAX_ROTABLE_NAME	32

typedef int luaR_numkey;

typedef struct {
  int type;                         /* LUA_TSTRING, LUA_TNUMBER or LUA_TNIL */
  union {
    const char *strkey;
    luaR_numkey numkey;
  } id;
} luaR_key;

typedef struct {
  const luaR_key key;
  const TValue value;               /* lightfunction, number, rotable or nil */
} luaR_entry;

/*
** A rotable is identified by a two-word magic at its start.  Values of
** type LUA_TROTABLE point to this structure, not directly to the entries.
** The magic lets the core tell a rotable from a normal Table* when it only
** has a void* (for example a metatable).
*/
#define LUA_ROTABLE_MAGIC1	0x4C524F54u	/* "LROT" */
#define LUA_ROTABLE_MAGIC2	0x41424C45u	/* "ABLE" */

typedef struct {
  unsigned int magic1;
  unsigned int magic2;
  const luaR_entry *pentries;
} luaR_table;

#define LUA_ROTABLE(entries)	{ LUA_ROTABLE_MAGIC1, LUA_ROTABLE_MAGIC2, entries }


/* Entry-definition helpers, compatible with both packed and plain TValue. */
#ifdef LUA_PACK_VALUE
#define LRO_FUNCVAL(v)	{ {(int)(v), add_sig(LUA_TLIGHTFUNCTION)} }
#define LRO_ROVAL(v)	{ {(int)(v), add_sig(LUA_TROTABLE)} }
#define LRO_NUMVAL(v)	{ .value.n = (v) }
#define LRO_NILVAL	{ {0, add_sig(LUA_TNIL)} }
#else
#define LRO_FUNCVAL(v)	{ { .p = (void *)(v) }, LUA_TLIGHTFUNCTION }
#define LRO_ROVAL(v)	{ { .p = (void *)(v) }, LUA_TROTABLE }
#define LRO_NUMVAL(v)	{ { .n = (v) }, LUA_TNUMBER }
#define LRO_NILVAL	{ { .p = NULL }, LUA_TNIL }
#endif

#define LRO_STRKEY(k)	{ LUA_TSTRING, { .strkey = (k) } }
#define LRO_NUMKEY(k)	{ LUA_TNUMBER, { .numkey = (k) } }
#define LRO_NILKEY	{ LUA_TNIL, { .strkey = NULL } }


int luaR_isrotable (const void *p);
const TValue *luaR_findentry (void *data, const char *strkey,
                              luaR_numkey numkey, unsigned *ppos);
void *luaR_getmeta (void *data);
void luaR_getcstr (char *dest, const TString *src, size_t maxsize);
void luaR_next (lua_State *L, void *data, TValue *key, TValue *val);


#endif
