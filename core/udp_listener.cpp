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
#include "udp_listener.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "error_code.h"
#include "logger.h"

namespace rs {
namespace core {

#define UDP_RECEIVE_BUFFER_SIZE 65535

RsUdpListener::RsUdpListener(string ip, int port, IUdpHandler* handler)
    : RsThread("udplistener")
{
    ip_addr = ip;
    listen_port = port;
    udp_handler = handler;

    buffer_size = UDP_RECEIVE_BUFFER_SIZE;
    recv_buffer = new char[UDP_RECEIVE_BUFFER_SIZE];
}

RsUdpListener::~RsUdpListener() {

}

int RsUdpListener::start_listen() {
    int ret = ERROR_SUCCESS;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(socket_fd == -1) {
        ret = ERROR_SOCKET_CREATE;
        return ret;
    }

    int reuse_flag = 1;
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_flag, sizeof(reuse_flag))) {
        ret = ERROR_SOCKET_SETREUSE;
        return ret;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    if(bind(socket_fd, (const sockaddr*)&addr, sizeof(addr)) == -1) {
        ret = ERROR_SOCKET_BIND;
        return ret;
    }

    st_socket_fd = st_netfd_open_socket(socket_fd);

    if(st_socket_fd == NULL) {
        ret = ERROR_ST_OPEN_SOCKET;
        return ret;
    }

    //call baseclass RsThread's method start_thread() to start the thread.
    start_thread();

    return ret;
}

int RsUdpListener::on_thread_start() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsUdpListener::on_before_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsUdpListener::loop() {
    int ret = ERROR_SUCCESS;
    sockaddr_in addr;
    int from_size = sizeof(sockaddr_in);
    int read_size = st_recvfrom(st_socket_fd, recv_buffer, buffer_size, (sockaddr*)&addr, &from_size, ST_UTIME_NO_TIMEOUT);

    if(read_size <= 0) {
        // ignore it since nothing received from the peer
        return ret;
    }

    if (ret = udp_handler->handle_udp_packet(st_socket_fd, &addr, recv_buffer, read_size) != ERROR_SUCCESS) {
        RSLOGE("accept client error. ret=%d", ret);
        return ret;
    }

    return ret;
}

int RsUdpListener::on_end_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsUdpListener::on_thread_stop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

} } // namespace rs::coree rs::core