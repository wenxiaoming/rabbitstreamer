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
#ifndef CORE_UDP_LISTENER_H_
#define CORE_UDP_LISTENER_H_

#include <string>
#include "st.h"
#include "thread.h"
#include "socket_handler.h"

using namespace std;

namespace rs {
namespace core {

class RsUdpListener: public RsThread {

public:
	RsUdpListener(string ip, int port, IUdpHandler* handler);
    ~RsUdpListener();

public:
    int start_listen();

public:
    // implement rs_thread's virtual function
    virtual int on_thread_start();
    virtual int on_before_loop();
    virtual int loop();
    virtual int on_end_loop();
    virtual int on_thread_stop();

private:
    int socket_fd;
    st_netfd_t st_socket_fd;

    string ip_addr;
    int listen_port;

    char* recv_buffer;
    int buffer_size;
    IUdpHandler* udp_handler;
};

} } // namespace rs::core

#endif /* CORE_UDP_LISTENER_H_ */
