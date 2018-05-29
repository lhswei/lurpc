#include <iostream>
#include <memory.h>
#include "lpublic.h"
#include "tcp_client.h"
#include <lua/lua.hpp>
#include <functional>
#include <string>

#pragma comment (lib, "Ws2_32.lib")


extern lua_State* luaEnv;

static int findfield_1(lua_State *L, int objidx, int level) {
	if (level == 0 || !lua_istable(L, -1))
		return 0;  /* not found */
	lua_pushnil(L);  /* start 'next' loop */
	while (lua_next(L, -2)) {  /* for each pair in table */
		if (lua_type(L, -2) == LUA_TSTRING) {  /* ignore non-string keys */
			if (lua_rawequal(L, objidx, -1)) {  /* found object? */
				lua_pop(L, 1);  /* remove value (but keep name) */
				return 1;
			}
			else if (findfield_1(L, objidx, level - 1)) {  /* try recursively */
				lua_remove(L, -2);  /* remove table (but keep name) */
				lua_pushliteral(L, ".");
				lua_insert(L, -2);  /* place '.' between the two names */
				lua_concat(L, 3);
				return 1;
			}
		}
		lua_pop(L, 1);  /* remove value */
	}
	return 0;  /* not found */
}

/*
** Search for a name for a function in all loaded modules
** (registry._LOADED).
*/
static int pushglobalfuncname_1(lua_State *L, lua_Debug *ar) {
	int top = lua_gettop(L);
	lua_getinfo(L, "f", ar);  /* push function */
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	if (findfield_1(L, top + 1, 2)) {
		const char *name = lua_tostring(L, -1);
		if (strncmp(name, "_G.", 3) == 0) {  /* name start with '_G.'? */
			lua_pushstring(L, name + 3);  /* push name without prefix */
			lua_remove(L, -2);  /* remove original name */
		}
		lua_copy(L, -1, top + 1);  /* move name to proper place */
		lua_pop(L, 2);  /* remove pushed values */
		return 1;
	}
	else {
		lua_settop(L, top);  /* remove function and global table */
		return 0;
	}
}


static void pushfuncname_1(lua_State *L, lua_Debug *ar) {
	if (pushglobalfuncname_1(L, ar)) {  /* try first a global name */
		lua_pushfstring(L, "function '%s'", lua_tostring(L, -1));
		lua_remove(L, -2);  /* remove name */
	}
	else if (*ar->namewhat != '\0')  /* is there a name from code? */
		lua_pushfstring(L, "%s '%s'", ar->namewhat, ar->name);  /* use it */
	else if (*ar->what == 'm')  /* main? */
		lua_pushliteral(L, "main chunk");
	else if (*ar->what != 'C')  /* for Lua functions, use <file:line> */
		lua_pushfstring(L, "function <%s:%d>", ar->short_src, ar->linedefined);
	else  /* nothing left... */
		lua_pushliteral(L, "?");
}

static int lastlevel_1(lua_State *L) {
	lua_Debug ar;
	int li = 1, le = 1;
	/* find an upper bound */
	while (lua_getstack(L, le, &ar)) { li = le; le *= 2; }
	/* do a binary search */
	while (li < le) {
		int m = (li + le) / 2;
		if (lua_getstack(L, m, &ar)) li = m + 1;
		else le = m;
	}
	return le - 1;
}

void luaL_traceback_1(lua_State *L, lua_State *L1,
	const char *msg, int level) {
	lua_Debug ar;
	int top = lua_gettop(L);
	int last = lastlevel_1(L1);
	int LEVELS1 = 10;
	int LEVELS2 = 11;
	int n1 = (last - level > LEVELS1 + LEVELS2) ? LEVELS1 : -1;
	if (msg)
		lua_pushfstring(L, "%s\n", msg);
	luaL_checkstack(L, 10, NULL);
	lua_pushliteral(L, "stack traceback:");
	while (lua_getstack(L1, level++, &ar)) {
		if (n1-- == 0) {  /* too many levels? */
			lua_pushliteral(L, "\n\t...");  /* add a '...' */
			level = last - LEVELS2 + 1;  /* and skip to last ones */
		}
		else {
			lua_getinfo(L1, "Slnt", &ar);
			lua_pushfstring(L, "\n\t%s:", ar.short_src);
			if (ar.currentline > 0)
				lua_pushfstring(L, "%d:", ar.currentline);
			lua_pushliteral(L, " in ");
			pushfuncname_1(L, &ar);
			if (ar.istailcall)
				lua_pushliteral(L, "\n\t(...tail calls...)");
			lua_concat(L, lua_gettop(L) - top);
		}
	}
	lua_concat(L, lua_gettop(L) - top);
}


void PrintLuaError(lua_State* L, int sErr)
{
	if (sErr == 0)
	{
		return;
	}
	const char* error;
	char sErrorType[256] = { 0 };
	switch (sErr)
	{
	case LUA_ERRSYNTAX://编译时错误  
					   /*const char *buf = "mylib.myfun()2222";类似这行语句可以引起编译时错误*/
		sprintf_s(sErrorType, sizeof(sErrorType), "%s", "syntax error during pre-compilation\n");
		break;
	case LUA_ERRMEM://内存错误  
		sprintf_s(sErrorType, sizeof(sErrorType), "%s", "memory allocation error\n");
		break;
	case LUA_ERRRUN://运行时错误  
					/*const char *buf = "my222lib.myfun()";类似这行语句可以引起运行时错误，my222lib实际上不存在这样的库，返回的值是nil*/
		sprintf_s(sErrorType, sizeof(sErrorType), "%s", "a runtime error\n");
		break;
	case LUA_YIELD://线程被挂起错误  
		sprintf_s(sErrorType, sizeof(sErrorType), "%s", "Thread has Suspended\n");
		break;
	case LUA_ERRERR://在进行错误处理时发生错误  
		sprintf_s(sErrorType, sizeof(sErrorType), "%s", "error while running the error handler function\n");
		break;
	default:
		break;
	}
	//error = lua_tostring(L, -1);//打印错误结果  
	luaL_traceback_1(L, L, NULL, 1);
	const char* error1 = lua_tostring(L, -1);//打印错误结果  
	printf("%s:%s\n", sErrorType, error1);
	lua_pop(L, 1);
}

static int traceback_L(lua_State *L) {
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	//---------------------------
	lua_pop(L, -2); //to popup the 'debug'
					//---------------------------
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);
	printf("%s\n", lua_tostring(L, -1));
	return 1;
}

LTcpClientTest::LTcpClientTest(const char* szIPAress, int nPort)
{
    memset(m_szIPAdress, 0, _TCP_CHAR_IP_LEN);
    if (szIPAress && strlen(szIPAress) > 0)
    {
        strncpy(m_szIPAdress, szIPAress, _TCP_CHAR_IP_LEN - 1);
    }

    m_nPort = _TCP_DEFAULT_SERVER_PORT;
    if (nPort > 1024 && nPort < 65534)
    {
        m_nPort = nPort;
    }

    m_socketConn = -1;
}

LTcpClientTest::~LTcpClientTest()
{
    Unit();
}

int LTcpClientTest::Init()
{
    int nResult = 0;
    int nRetCode = 0;
    WSADATA wsaData;

    // Initialize Winsock
    LU_PROCESS_ERROR(WSAStartup(MAKEWORD(2,2), &wsaData) == 0);
    LU_PROCESS_ERROR(m_szIPAdress[0]);
    LU_PROCESS_ERROR(m_nPort > 1024 && m_nPort < 65534);

    m_socketConn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    LU_PROCESS_ERROR(m_socketConn >= 0);

    memset(&m_server_addr, 0, sizeof(m_server_addr));
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(m_nPort);
	m_server_addr.sin_addr.s_addr = inet_addr(m_szIPAdress);
    //nRetCode = inet_pton(AF_INET, m_szIPAdress, &m_server_addr.sin_addr);
    //LU_PROCESS_ERROR(nRetCode > 0);

    nResult = 1;
Exit0:

    return nResult;
}

void LTcpClientTest::Unit()
{
    if (m_socketConn >= 0)
    {
        closesocket(m_socketConn);
        m_socketConn = INVALID_SOCKET;
        WSACleanup();
    }
}

void LTcpClientTest::InitScriptLib()
{
	static int& conn = m_socketConn;
	lua_CFunction fun = [](lua_State* L)->int {
		size_t len = 0;
		char szMessage[_TCP_MAX_BUFFER_SIZE];
		const char* msg = luaL_checklstring(L, 1, &len);
		int nRetCode = send(conn, msg, len, 0);
		printf("send result = %d\n", nRetCode);
		memset(szMessage, 0, _TCP_MAX_BUFFER_SIZE);
		int len1 = recv(conn, szMessage, _TCP_MAX_BUFFER_SIZE - 1, 0);
		if (len1 > 0)
			lua_pushstring(luaEnv, szMessage);
		else
			lua_pushnil(luaEnv);
		return 1;
	};

	struct luaL_Reg lib[] =
	{
		{ "rpc_call", fun },
	{ NULL, NULL }
	};

	luaL_register(luaEnv, "glib", lib);
}

int LTcpClientTest::Connect2Server()
{
    int nResult = 0;
    int nRetCode = 0;

    nRetCode = connect(m_socketConn, (sockaddr*)&m_server_addr, sizeof(m_server_addr));
    LU_PROCESS_ERROR(nRetCode >= 0);

    nResult = 1;
Exit0:
    printf("tcpserver Connect2Server result = %d\n", nResult);
    return nResult;
}

void LTcpClientTest::Run()
{
    char szMessage[_TCP_MAX_BUFFER_SIZE];
    int nRetCode = 0;
    while(true)
    {
        printf("send message: ");
        memset(szMessage, 0, _TCP_MAX_BUFFER_SIZE);
        fgets(szMessage, _TCP_MAX_BUFFER_SIZE - 1, stdin);
		//lua_pushcfunction(luaEnv, traceback_L);
		int nRet = luaL_dostring(luaEnv, szMessage);
		if (nRet != 0)
			PrintLuaError(luaEnv, nRet);
			//traceback_L(luaEnv);

        L_SLEEP(1);
    }
}








