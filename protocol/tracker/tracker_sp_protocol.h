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
#ifndef PROTOCOL_TRACKER_SP_PROTOCOL_H_
#define PROTOCOL_TRACKER_SP_PROTOCOL_H_

#include <st.h>
#include <string>
#include <stdint.h>
#include "core/thread.h"
#include "core/socket.h"
#include "core/core_struct.h"
#include "core/core_utility.h"
#include "core/timer.h"
#include "core/struct_define.h"

using namespace std;

using namespace core;

//class map_str;

namespace protocol {
namespace tracker {

class RsSpTracker : public RsThread,
                    public virtual ITimerHandler
{
public:
	RsSpTracker();
	virtual ~RsSpTracker();
public:
    //implement rs_thread's virtual function
    virtual int on_thread_start();
    virtual int on_before_loop();
    virtual int loop();
    virtual int on_end_loop();
    virtual int on_thread_stop();
private:
	void generate_uuid(map_str& digits);
    int send_buffer(char* buf, int size);
public:
    //implement ITimerHandler
    virtual int handle_timeout(int64_t timerid);
public:
    int handle_udp_packet(st_netfd_t st_fd, sockaddr_in* from, char* buf, int nb_buf);
private:
    char* recv_buf;
    int buf_size;
    bool register_flag;
    int register_retry;
private:
    int64_t last_thread_time;
private:
    //string ip_address;
    //int ip_port;
    st_netfd_t sp_fd;
    RsSocket* io;
	sockaddr_in last_receive_addr;
protected:
    int send_welcome(map_str uuid);
    int send_sp_list(map_str uuid);
    int send_errormsg();
    int send_res_interval();

	//handle message from sp to tracker
	int get_register(char* msg, int size);
    int get_res_list(char* msg, int size);
    int get_sp_list(char* msg, int size);
    int get_status(char* msg, int size);
    int get_logout(char* msg, int size);
    // uuid of super peer on tracker
    char sp_id[UUID_LENGTH];
    // recved welcome
    bool login_done_;
};


} /* namespace protocol */
} /* namespace tracker  */

#endif
