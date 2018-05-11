#include <stdlib.h>
#include <stdio.h>
#include <direct.h> 
#include <iostream>
#include <memory.h>
#include "server.hpp"
#include "lpublic.h"
#include "tcp_servertest.h"

lua_State* luaEnv = nullptr;
const char* s_default_ini_script = "./script/server/init.lua";

int init_script()
{
    int ret = 0;
    if (!luaEnv)
        return ret;
    ret = luaL_dofile(L, s_default_ini_script);
    return ret;
}

int init_lua()
{
    luaEnv = luaL_newstate();
    if (!luaEnv)
    {
        printf("lua_open fail!\n");
        getchar();
        return 0;
    }

    luaL_openlibs(luaEnv);
    return 1;
}
int main(int argc, char *argv[])
{
    init_lua()
    init_script()
    if (luaEnv)
    {
        LTcpServerTest tcpServer(_TCPTEST_DEFAULT_SERVER_PORT);
        if (tcpServer.Init() == 1)
        {
            tcpServer.Run();
        }
    }

    lua_close(luaEnv);
    getchar();
    return 0;
}