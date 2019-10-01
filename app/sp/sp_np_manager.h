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

#ifndef APP_SP_NP_MANAGER_H_
#define APP_SP_NP_MANAGER_H_

#include "core/tcp_listener.h"
#include "protocol/sp/sp_np_protocol.h"
#include <string>
#include <vector>

using namespace rs::core;
using namespace rs::protocol::sp;

namespace rs {
namespace app {
namespace sp {

class SpNpManager : public ITcpHandler {

public:
    SpNpManager(std::string ip, uint32_t port);
    ~SpNpManager();

public:
    int start_listener();

public:
    // implement interface ITcpHandler
    virtual int handle_tcp_connect(st_netfd_t stfd);

private:
    RsTcpListener* tcp_listener;
    string ip_addr;
    uint32_t listen_port;
    std::vector<RsNpSpProtocol*> np_sp_protocol_vector;
};

} } } // namespace rabbitstreamer::app::sp

#endif /* APP_SP_NP_MANAGER_H */
