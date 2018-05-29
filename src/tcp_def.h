// 
// @filename:    tcp_testdef.h  
// @Author:      luhengsi
// @DateTime:    2017-11-07 20:27:23
// @Description: tcp server
// @Description: 

#ifndef _LU_TCP_DEF_H_
#define _LU_TCP_DEF_H_

#ifdef __linux
#include <unistd.h>
#else
#include <synchapi.h >
#endif

#define _TCP_DEFAULT_SERVER_PORT    8869
#define _TCP_MAX_CONN_WAIT          10
#define _TCP_CHAR_IP_LEN            33
#define _TCP_MAX_BUFFER_SIZE        (1024 * 4) 


#ifndef L_SLEEP(s)
#ifdef __linux
#define L_SLEEP(s) usleep((s)*1000)
#else
#define L_SLEEP(s) Sleep((s))
#endif //__linux
#endif //L_SLEEP(S)






















#endif //_LU_TCP_DEF_H_


