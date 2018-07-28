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
#include "tcp_listener.h"
#include "error_code.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

// nginx also set to 512
#define SERVER_LISTEN_BACKLOG 512


RsTcpListener::RsTcpListener(string ip, int port, ITcpHandler* handler)
{
   ip_addr = ip;
   listen_port = port;
   tcp_handler = handler;
}

RsTcpListener::~RsTcpListener()
{

}

int RsTcpListener::start_listen()
{
    int ret = ERROR_SUCCESS;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_fd == -1)
    {
        ret = ERROR_SOCKET_CREATE;
        return ret;
    }

    int reuse_flag = 1;
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(reuse_flag))== -1)
    {
        ret = ERROR_SOCKET_SETREUSE;
        return ret;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    if(bind(socket_fd, (const sockaddr*)&addr, sizeof(addr)) == -1)
    {
        ret = ERROR_SOCKET_BIND;
        return ret;
    }

   if(::listen(socket_fd, SERVER_LISTEN_BACKLOG)==-1)
    {
        ret = ERROR_SOCKET_LISTEN;
        return ret;
    }

    st_socket_fd = st_netfd_open_socket(socket_fd);

    if(st_socket_fd == NULL)
    {
        ret = ERROR_ST_OPEN_SOCKET;
        return ret;
    }

    //call baseclass RsThread's method start_thread() to start the thread.
    start_thread();

    return ret;
}

int RsTcpListener::on_thread_start()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTcpListener::on_before_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTcpListener::loop()
{
    int ret = ERROR_SUCCESS;

    st_netfd_t client_stfd = st_accept(st_socket_fd, NULL, NULL, ST_UTIME_NO_TIMEOUT);

    if(client_stfd == NULL){
        // ignore error.
        if (errno != EINTR) {
            //srs_error("ignore accept thread stoppped for accept client error");
        }
        return ret;
    }
    //srs_verbose("get a client. fd=%d", st_netfd_fileno(client_stfd));

    if ((ret = tcp_handler->handle_tcp_connect(client_stfd)) != ERROR_SUCCESS) {
        //srs_warn("accept client error. ret=%d", ret);
     //   return ret;
    }

    return ret;
}

int RsTcpListener::on_end_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsTcpListener::on_thread_stop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}


