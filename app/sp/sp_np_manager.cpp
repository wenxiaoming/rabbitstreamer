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

#include "sp_np_manager.h"
#include "core/resource.h"

namespace rs {
namespace app {
namespace sp {

SpNpManager::SpNpManager(string ip, uint32_t port)
    : ip_addr(ip), listen_port(port) {}

SpNpManager::~SpNpManager() {}

int SpNpManager::start_listener() {
    tcp_listener = make_unique_ptr<RsTcpListener>(ip_addr, listen_port, this);
    tcp_listener->start_listen();
    return 0;
}

int SpNpManager::handle_tcp_connect(st_netfd_t stfd) {
    std::unique_ptr<RsNpSpProtocol> np_sp_protocol =
        make_unique_ptr<RsNpSpProtocol>(stfd);
    np_sp_protocol->start_thread();
    np_sp_protocol_vector.push_back(std::move(np_sp_protocol));
    return 0;
}

} // namespace sp
} // namespace app
} // namespace rs
