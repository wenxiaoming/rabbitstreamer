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
#ifndef PROTOCOL_TRACKER_NP_PROTOCOL_H_
#define PROTOCOL_TRACKER_NP_PROTOCOL_H_

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

class RsNpTracker : public RsThread,
                            public virtual ITimerHandler
{
public:
	RsNpTracker();
	virtual ~RsNpTracker();
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
private:
	int get_login(char* msg, int size);
	int get_req_res(char* msg, int size);
	int get_report(char* msg, int size);
	int get_need_peers(char* msg, int size);
	int get_logout(char* msg, int size);
	int get_res_interval(char* msg, int size);

	int send_peers(map_str uuid, MD5_Hash_Str resHash, uint32_t currentblockID=0);
	int send_welcome(map_str digits, P2PAddress p2pAddr);
	int send_res_interval(MD5_Hash_Str channel_hash);
	void send_msg();
private:
	BlockInterval last_send_blockinterval;
};

#endif
