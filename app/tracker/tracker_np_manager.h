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
#ifndef APP_TRACKER_NP_MANAGER_H_
#define APP_TRACKER_NP_MANAGER_H_

#include "core/udp_listener.h"
#include "protocol/tracker/tracker_np_connector.h"

using namespace rs::protocol::tracker;

namespace rs {
namespace app {
namespace tracker {

class TrackerNpManager : public IUdpHandler {
public:
    TrackerNpManager(string ip, int port);
    ~TrackerNpManager();

public:
    int start_listener();

public:
    // implement interface IUdpHandler
    virtual int handle_udp_packet(st_netfd_t st_fd, sockaddr_in* from, char* buf, int nb_buf);

private:
    RsUdpListener* udp_listener;
    string ip_addr;
    int listen_port;
    RsNpTracker* np_tracker;
};

} } } // namespace rabbitstreamer::app::tracker

#endif /* APP_TRACKER_NP_MANAGER_H_ */
