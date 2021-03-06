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

#ifndef CORE_SOCKET_HANDLER_H_
#define CORE_SOCKET_HANDLER_H_

#include <st.h>
#include <arpa/inet.h>

namespace rs {
namespace core {

class ITcpHandler {
  public:
    ITcpHandler() {}
    virtual ~ITcpHandler() {}

  public:
    virtual int handle_tcp_connect(st_netfd_t stfd) = 0;
};

class IUdpHandler {
  public:
    IUdpHandler() {}
    virtual ~IUdpHandler() {}

  public:
    virtual int handle_udp_packet(st_netfd_t st_fd, sockaddr_in *from,
                                  char *buf, int nb_buf) = 0;
};

} // namespace core
} // namespace rs

#endif /* CORE_RS_SOCKET_HANDLER_H_ */
