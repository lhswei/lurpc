#include <iostream>
#include <memory.h>
#include "lpublic.h"
#include "tcp_client.h"
#include <lua/lua.hpp>
#include <functional>
#include <string>

#pragma comment (lib, "Ws2_32.lib")


extern lua_State* luaEnv;

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
		int nRet = luaL_dostring(luaEnv, szMessage);
        L_SLEEP(1);
    }
}








