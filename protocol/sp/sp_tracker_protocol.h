/*
The MIT License (MIT)

Copyright (c) 2016-2017 RabbitStreamer

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

#ifndef PROTOCOL_SP_TRACKER_PROTOCOL_H_
#define PROTOCOL_SP_TRACKER_PROTOCOL_H_

#include <st.h>
#include <string>
#include <stdint.h>
#include "core/thread.h"
#include "core/socket.h"
#include "core/core_struct.h"
#include "core/core_utility.h"
#include "core/timer.h"

using namespace std;

using namespace core;

namespace protocol {
namespace sp {

class RsSpTrackerProtocol : public RsThread,
                            public virtual ITimerHandler
{
public:
	RsSpTrackerProtocol(string ip, int port);
	virtual ~RsSpTrackerProtocol();
public:
	int start_connect();
public:
    //implement rs_thread's virtual function
    virtual int on_thread_start();
    virtual int on_before_loop();
    virtual int loop();
    virtual int on_end_loop();
    virtual int on_thread_stop();
public:
    //implement ITimerHandler
    virtual int handle_timeout(int64_t timerid);

private:
    char* recv_buf;
    int buf_size;
    bool register_flag;
    int register_retry;
private:
    int64_t last_thread_time;
private:
    string ip_address;
    int ip_port;
    st_netfd_t tracker_udp_fd;
    RsSocket* io;
protected:
    int get_welcome(char* msg, int size);
    int get_sp_list(char* msg, int size);
    int get_res_interval(char* msg, int size);

    int send_register();
    int send_res_list();
    int send_sp_list();
    int send_status();
    int send_logout();

    // uuid of super peer on tracker
    char sp_id[UUID_LENGTH];

    // recved welcome
    bool login_done_;

    // sp port
    uint16_t sp_port;
};


} /* namespace protocol */
} /* namespace sp  */

#endif /* PROTOCOL_SP_TRACKER_PROTOCOL_H_ */
