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
#include "core/error_code.h"
#include "core/message.h"
#include "core/p2p_protocol.h"
#include "core/socket_connect.h"
#include "sp_source_manager.h"
#include "sp_tracker_connector.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <st.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace rs {
namespace protocol {
namespace sp {

#define TRACKER_UDP_CONNECT_TIMEOUT_US (int64_t)(1 * 1000 * 1000LL)

#define UDP_MAX_PACKET_SIZE 65535

#define UDP_PACKET_RECV_CYCLE_INTERVAL_MS 0

#define TRACKER_TIMEOUT 1000                  // timeout is 1s
#define TRACKER_GET_SP_LIST_TIMEOUT 50 * 1000 // timeout is 50s

#define TRACKER_TIMER_ID 0
#define TRACKER_GET_SP_LIST_TIMER_ID 1

RsSpTrackerProtocol::RsSpTrackerProtocol(string ip, int port)
    : RsThread("trackerprotocol") {
    ip_address = ip;
    ip_port = port;
    sp_port = 2222; // hard code the port for np to access sp through tcp
    buf_size = UDP_MAX_PACKET_SIZE;
    recv_buf = new char[UDP_MAX_PACKET_SIZE];
    register_flag = false;
    register_retry = 0;

    cycle_interval_us = 120 * 1000; // sleep 120ms
}

RsSpTrackerProtocol::~RsSpTrackerProtocol() {
    if (io)
        delete io;

    if (recv_buf)
        delete[] recv_buf;
}

int RsSpTrackerProtocol::start_connect() {
    int ret = ERROR_SUCCESS;
    // open socket.
    int64_t timeout = TRACKER_UDP_CONNECT_TIMEOUT_US;
    // register two timers
    RsTimer::instance()->add_timer(TRACKER_TIMEOUT, TRACKER_TIMER_ID, this);
    RsTimer::instance()->add_timer(TRACKER_GET_SP_LIST_TIMEOUT,
                                   TRACKER_GET_SP_LIST_TIMER_ID, this);

    bool is_tcp = false;

    ret = socket_connect(is_tcp, ip_address, ip_port,
                         TRACKER_UDP_CONNECT_TIMEOUT_US, &tracker_udp_fd);

    if (ERROR_SUCCESS == ret) {
        io = new RsSocket(tracker_udp_fd);
        start_thread();
    }

    return ret;
}

int RsSpTrackerProtocol::on_end_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTrackerProtocol::on_thread_stop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTrackerProtocol::on_thread_start() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTrackerProtocol::on_before_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTrackerProtocol::send_register() {
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsRegister register_msg;
    register_msg.service_port = sp_port;
    char *payload = NULL;
    int payload_nb = 0;
    // RsMessage* msg = register_msg;
    register_msg.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        io->write(payload, payload_nb, &nsize);
    st_usleep(1000); // release CPU cycle to others
    return ret;
}

int RsSpTrackerProtocol::send_res_list() {
    int ret = ERROR_SUCCESS;
    list<source_status> source_list;
    RsSourceManager::instance()->get_source_list(source_list);
    Sp2TsResList res_list;
    memcpy(res_list.sp_uuid, sp_id, UUID_LENGTH);
    res_list.resource_count = source_list.size();

    if (!res_list.resource_count)
        return ret;

    // printf("%s\n", __FUNCTION__);
    int index = 0;
    res_list.res_info = new resource_info[res_list.resource_count];
    // update res list for each source
    list<source_status>::iterator iter = source_list.begin();
    for (; iter != source_list.end(); iter++) {
        memcpy(res_list.res_info[index].res_md5, iter->chnl_hash.hash_,
               MD5_LEN);
        res_list.res_info[index].block_interval = iter->block_inter_;
        index++;
    }

    char *payload = NULL;
    int payload_nb = 0;
    res_list.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        io->write(payload, payload_nb, &nsize);

    return ret;
}

int RsSpTrackerProtocol::send_sp_list() {
    // printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsSpList sp_list;
    memcpy(sp_list.sp_uuid, sp_id, UUID_LENGTH);
    char *payload = NULL;
    int payload_nb = 0;
    sp_list.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        io->write(payload, payload_nb, &nsize);
    return ret;
}

int RsSpTrackerProtocol::get_welcome(char *msg, int size) {
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Ts2SpWelcome welcome_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    welcome_msg.parse(&streamer);
    memcpy(sp_id, welcome_msg.sp_uuid, UUID_LENGTH);
    register_flag = true;
    return ret;
}

int RsSpTrackerProtocol::get_sp_list(char *msg, int size) {
    int ret = ERROR_SUCCESS;
    printf("%s\n", __FUNCTION__);
    return ret;
}

int RsSpTrackerProtocol::get_res_interval(char *msg, int size) {
    int ret = ERROR_SUCCESS;
    // printf("%s\n", __FUNCTION__);
    if (register_flag)
        send_res_list();
    else
        send_register();
    return ret;
}

int RsSpTrackerProtocol::loop() {
    int ret = ERROR_SUCCESS;

    sockaddr_in from;
    int from_len = sizeof(sockaddr_in);
    int read_size = 0;

    if ((read_size =
             st_recvfrom(tracker_udp_fd, recv_buf, buf_size, (sockaddr *)&from,
                         &from_len, ST_UTIME_NO_TIMEOUT)) <= 0) {
        return ret;
    }

    // get message size
    char *temp = recv_buf;
    int msg_size = 0;
    memcpy((char *)&msg_size, recv_buf, 4);
    recv_buf += 4;

    // get the message type
    char msg_type = 0;
    msg_type = *recv_buf;
    recv_buf += 1;

    switch (msg_type) {
    case TS2SP_WELCOME:
        get_welcome(temp + 5, msg_size - 5);
        break;
    case TS2SP_SP_LIST:
        get_sp_list(temp + 5, msg_size - 5);
        break;
    case TS2SP_GET_RES_LIST:
        get_res_interval(temp + 5, msg_size - 5);
        break;
    }

    if (UDP_PACKET_RECV_CYCLE_INTERVAL_MS > 0) {
        st_usleep(UDP_PACKET_RECV_CYCLE_INTERVAL_MS * 1000);
    }
    return ret;
}

int RsSpTrackerProtocol::handle_timeout(int64_t timerid) {
    int ret = ERROR_SUCCESS;

    if (!register_flag) // send register info if not register
        return send_register();

    switch (timerid) {
    case TRACKER_TIMER_ID:
        send_res_list();
        break;
    case TRACKER_GET_SP_LIST_TIMER_ID:
        send_sp_list();
        break;
    }
    return ret;
}

} // namespace sp
} // namespace protocol
} // namespace rs
