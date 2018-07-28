/*
The MIT License (MIT)

Copyright (c) 2016-2018 RabbitStreamer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "st.h"
#include "socket.h"
#include "socket_connect.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "error_code.h"
#include "logger.h"

int socket_connect(int socket_type, string server, int port, int64_t timeout, st_netfd_t* pstfd)
{
    int ret = ERROR_SUCCESS;

    *pstfd = NULL;
    st_netfd_t stfd = NULL;
    sockaddr_in addr;

    int sock = 0;
    if(socket_type)
        sock = socket(AF_INET, SOCK_STREAM, 0);
    else
        sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock == -1){
        ret = ERROR_SOCKET_CREATE;
        RSLOGE("create socket error. ret=%d", ret);
        return ret;
    }

    stfd = st_netfd_open_socket(sock);
    if(stfd == NULL){
        ret = ERROR_ST_OPEN_SOCKET;
        RSLOGE("st_netfd_open_socket failed. ret=%d", ret);
        return ret;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server.c_str());

    if (st_connect(stfd, (const struct sockaddr*)&addr, sizeof(sockaddr_in), timeout) == -1){
        ret = ERROR_ST_CONNECT;
        RSLOGE("connect to server error. ip=%s, port=%d, ret=%d", server.c_str(), port, ret);
        goto failed;
    }

    RSLOGE("connect ok. server=%s, ip=%s, port=%d", server.c_str(), server.c_str(), port);

    *pstfd = stfd;
    return ret;

failed:
    if (stfd) {
        rs_close_stfd(stfd);
    }
    return ret;
}
