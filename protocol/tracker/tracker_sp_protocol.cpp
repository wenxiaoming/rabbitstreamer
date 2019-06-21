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
#include "tracker_sp_protocol.h"
#include "tracker_streamer_manager.h"
#include "core/error_code.h"
#include "core/socket_connect.h"
#include "core/message.h"
#include "core/p2p_protocol.h"
#include "core/logger.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <st.h>

namespace protocol
{
namespace tracker
{

#define TRACKER_UDP_CONNECT_TIMEOUT_US (int64_t)(1 * 1000 * 1000LL)

#define UDP_MAX_PACKET_SIZE 65535

#define UDP_PACKET_RECV_CYCLE_INTERVAL_MS 0

#define TRACKER_TIMEOUT 1000                  //timeout is 1s
#define TRACKER_GET_SP_LIST_TIMEOUT 50 * 1000 //timeout is 50s

#define TRACKER_TIMER_ID 0
#define TRACKER_GET_SP_LIST_TIMER_ID 1

RsSpTracker::RsSpTracker()
    : RsThread("sptracker")
{
    // TODO Auto-generated constructor stub
    buf_size = UDP_MAX_PACKET_SIZE;
    recv_buf = new char[UDP_MAX_PACKET_SIZE];
    register_flag = false;
    register_retry = 0;

    io = new RsSocket(sp_fd);

    cycle_interval_us = 120 * 1000; //sleep 120ms
}

RsSpTracker::~RsSpTracker()
{
    // TODO Auto-generated destructor stub
    if (io)
        delete io;

    if (recv_buf)
        delete[] recv_buf;
}

int RsSpTracker::on_end_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTracker::on_thread_stop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTracker::on_thread_start()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsSpTracker::on_before_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

void RsSpTracker::generate_uuid(map_str &digits)
{
    int r1 = rand();
    int r2 = rand();
    int r3 = rand();
    int r4 = rand();

    sprintf(digits.str_, "%04x%04x%04x%04x", r1, r2, r3, r4);
}

int RsSpTracker::get_register(char *msg, int size)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsRegister register_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    register_msg.parse(&streamer);

    StreamMgr *mgr = StreamMgr::instance();
    //
    map_str uuid;
    generate_uuid(uuid);

    ChannelNode pNode;

    time(&pNode.last_recv_report_time_);
    pNode.userID = register_msg.user_id;

    strncpy((char *)pNode.pswd, register_msg.password, MD5_LEN);

    pNode.pswd[MD5_LEN] = 0;

    pNode.servicePort = register_msg.service_port;

    pNode.spAddress.sin_port = last_receive_addr.sin_port;
    pNode.spAddress.sin_addr.s_addr = last_receive_addr.sin_addr.s_addr;
    pNode.spService = this;

    //
    mgr->insert_channel(uuid, &pNode);

    return send_welcome(uuid);
}

int RsSpTracker::get_res_list(char *msg, int size)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsResList res_list_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    res_list_msg.parse(&streamer);

    map_str uuid;
    memcpy(uuid.str_, res_list_msg.sp_uuid, UUID_LENGTH);
    uuid.str_[UUID_LENGTH] = 0;

    ChannelNode pNode;
    StreamMgr *mgr = StreamMgr::instance();

    if (mgr->get_channel(uuid, &pNode) == -1)
    {
        printf("no resource UUID(16 bytes) channelNode| on_SP2TS_STATUS\n");
        send_errormsg();
        return 0;
    }

    time(&pNode.last_recv_report_time_);
    pNode.resourceCount = res_list_msg.resource_count;

    pNode.new_pHash();

    //resource MD5(MD5_LEN)
    for (int i = 0; i < pNode.resourceCount; i++)
    {
        memcpy(pNode.pHash[i].hash_, res_list_msg.res_info[i].res_md5, MD5_LEN);
        pNode.pHash[i].hash_[MD5_LEN] = 0;
        pNode.pHash[i].blockInterval = res_list_msg.res_info[i].block_interval;
        RSLOGE("index:%d block_interval start %d size %d\n", i, pNode.pHash[i].blockInterval.start, pNode.pHash[i].blockInterval.size);
    }

    mgr->insert_channel(uuid, &pNode);
}

int RsSpTracker::get_sp_list(char *msg, int size)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsSpList sp_list_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    sp_list_msg.parse(&streamer);

    map_str uuid;
    memcpy(uuid.str_, sp_list_msg.sp_uuid, UUID_LENGTH);
    uuid.str_[UUID_LENGTH] = 0;

    send_sp_list(uuid);
}

int RsSpTracker::get_status(char *msg, int size)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;

    Sp2TsStatus status_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    status_msg.parse(&streamer);

    map_str uuid;
    memcpy(uuid.str_, status_msg.sp_uuid, UUID_LENGTH);
    uuid.str_[UUID_LENGTH] = 0;

    ChannelNode pNode;
    StreamMgr *mgr = StreamMgr::instance();

    if (mgr->get_channel(uuid, &pNode) == -1)
    {
        printf("no resource UUID(16 bytes) channelNode| on_SP2TS_STATUS\n");
        send_errormsg();
        return 0;
    }

    time(&pNode.last_recv_report_time_);

    pNode.resourceCount = status_msg.resource_count;
    pNode.conNum = status_msg.connection_num;
    pNode.bandwidth = status_msg.bandwidth;
    pNode.exceedMaxConn = status_msg.exceed_max_connection;

    mgr->insert_channel(uuid, &pNode);
    return ret;
}

int RsSpTracker::get_logout(char *msg, int size)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;
    Sp2TsLogout logout_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    logout_msg.parse(&streamer);
}

int RsSpTracker::send_errormsg()
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int RsSpTracker::send_sp_list(map_str uuid)
{
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;

    NetAddress *spaddr = NULL;
    int count = 0;

    StreamMgr *mgr = StreamMgr::instance();

    mgr->get_cp_address(spaddr, count, uuid);

    if (count <= 0)
    {
        if (NULL != spaddr)
            delete[] spaddr;
        return 0;
    }
    Ts2SpSpList sp_list;
    sp_list.count = count;
    sp_list.addr = spaddr;

    char *payload = NULL;
    int payload_nb = 0;
    sp_list.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        io->write(payload, payload_nb, &nsize);

    if (NULL != spaddr)
        delete[] spaddr;

    return ret;
}

int RsSpTracker::loop()
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int RsSpTracker::handle_udp_packet(st_netfd_t st_fd, sockaddr_in *from, char *buf, int nb_buf)
{
    int ret = ERROR_SUCCESS;

    if (buf == NULL || nb_buf == 0)
        return -1; //fixme, error code

    sp_fd = st_fd;
    //save the remote address
    last_receive_addr = *from;
    //get message size
    char *temp = buf;
    int msg_size = 0; //(temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0];
    memcpy((char *)&msg_size, buf, 4);
    buf += 4;

    //get the message type
    char msg_type = 0;
    msg_type = *buf;
    buf += 1;

    RSLOGE("%s type:%d size:%d\n", __PRETTY_FUNCTION__, msg_type, msg_size);
    switch (msg_type)
    {
    case SP2TS_REGISTER:
        get_register(temp + 5, msg_size - 5);
        break;
    case SP2TS_STATUS:
        get_status(temp + 5, msg_size - 5);
        break;
    case SP2TS_RES_LIST:
        get_res_list(temp + 5, msg_size - 5);
        break;
    case SP2TS_GET_SP:
        get_sp_list(temp + 5, msg_size - 5);
        break;
    case SP2TS_LOGOUT:
        break;
    }

    if (UDP_PACKET_RECV_CYCLE_INTERVAL_MS > 0)
    {
        st_usleep(UDP_PACKET_RECV_CYCLE_INTERVAL_MS * 1000);
    }
    return ret;
}

int RsSpTracker::handle_timeout(int64_t timerid)
{
    int ret = ERROR_SUCCESS;

    switch (timerid)
    {
    case TRACKER_TIMER_ID:
        //send_res_list();
        break;
    case TRACKER_GET_SP_LIST_TIMER_ID:
        //send_sp_list();
        break;
    }
    return ret;
}

int RsSpTracker::send_buffer(char *buf, int size)
{
    int ret = ERROR_SUCCESS;
    ssize_t nsize = 0;
    if ((buf != NULL) && (size != 0))
        nsize = st_sendto(sp_fd, buf, size, (sockaddr *)&last_receive_addr, sizeof(last_receive_addr), ST_UTIME_NO_TIMEOUT);

    if (nsize != size)
        return -1; //todo, Kevin, handle error code
    return ret;
}

int RsSpTracker::send_welcome(map_str uuid)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    RSLOGE("%s\n", __PRETTY_FUNCTION__);
    int ret = ERROR_SUCCESS;
    Ts2SpWelcome welcome_msg;
    memcpy(welcome_msg.sp_uuid, uuid.str_, UUID_LENGTH);

    char *payload = NULL;
    int payload_nb = 0;
    welcome_msg.pack(payload, payload_nb);

    send_buffer(payload, payload_nb);
}

int RsSpTracker::send_res_interval()
{
}

} // namespace tracker
} // namespace protocol