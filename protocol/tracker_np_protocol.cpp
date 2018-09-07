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
#include "tracker_np_protocol.h"
#include "tracker_streamer_manager.h"
#include "tracker_np_manager.h"
#include "core/error_code.h"
#include "core/socket_connect.h"
#include "core/message.h"
#include "core/logger.h"
#include "core/p2p_protocol.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <st.h>

#define TRACKER_UDP_CONNECT_TIMEOUT_US (int64_t)(1*1000*1000LL)

#define UDP_MAX_PACKET_SIZE 65535

#define UDP_PACKET_RECV_CYCLE_INTERVAL_MS 0

#define TRACKER_TIMEOUT 1000  //timeout is 1s
#define TRACKER_GET_SP_LIST_TIMEOUT 50*1000 //timeout is 50s

#define TRACKER_TIMER_ID 0
#define TRACKER_GET_SP_LIST_TIMER_ID 1

RsNpTracker::RsNpTracker()
            : RsThread("nptracker")
{
	// TODO Auto-generated constructor stub
	buf_size = UDP_MAX_PACKET_SIZE;
    recv_buf = new char[UDP_MAX_PACKET_SIZE];
    register_flag = false;
    register_retry = 0;

	io = new RsSocket(sp_fd);

    cycle_interval_us = 120*1000;//sleep 120ms
}

RsNpTracker::~RsNpTracker()
{
	// TODO Auto-generated destructor stub
    if(io)
        delete io;

    if(recv_buf)
        delete[] recv_buf;
}

int RsNpTracker::on_end_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpTracker::on_thread_stop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpTracker::on_thread_start()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpTracker::on_before_loop()
{
    int ret = ERROR_SUCCESS;

    return ret;
}

void RsNpTracker::generate_uuid(map_str& digits)
{
	int r1 = rand();
	int r2 = rand();
	int r3 = rand();
	int r4 = rand();

	sprintf(digits.str_, "%04x%04x%04x%04x", r1, r2, r3, r4);
}

int RsNpTracker::get_login(char* msg, int size)
{
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsLogin login_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    login_msg.parse(&streamer);
	
	RSLOGE("NP:  LOGIN recieved  NP2TS_LOGIN MSG |on_NP2TS_LOGIN.\n");

	NPNode node;
	
	map_str digits;
	generate_uuid(digits);
	
	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();
	

	//login id(UINT32)
	uint32_t id = login_msg.login_id;

	//md5 password(MD5_LEN)
	char password[33];
	memcpy(password, login_msg.password, MD5_LEN);
	password[32] = 0;	
	
	//version of client(float)
	node.fVersion = login_msg.client_version;
	node.port = login_msg.listening_port;

	//size of local ip list(UINT8)
	uint8_t count = login_msg.local_ip_list_size;
	if(count == 0)
	{
		RSLOGE("no resource size of local ip list(UINT8) in on_NP2TS_LOGIN\n");
		return 0;
	}

	NetAddress remoteNormal;
	remoteNormal.sin_port = last_receive_addr.sin_port;//Kevin, FIMXE, add htons or not?
	remoteNormal.sin_addr.s_addr = last_receive_addr.sin_addr.s_addr;//Kevin, FIXME, add htons or not?

	// ip addr(in_addr)
	NetAddress* addr = new NetAddress[count];
	for (int i =0; i < count; i++)
	{
		addr[i].sin_addr = login_msg.ip_addr[i];
	}

	node.clientAddress.outerIP = remoteNormal;
    NetAddress natAddr;
	for (int n =0; n < count; n++)
	{
		if (memcmp(&addr[n].sin_addr, &remoteNormal.sin_addr, sizeof(remoteNormal.sin_addr)) == 0)
		{
			node.clientAddress.subnetIP.sin_port = node.port;
			node.clientAddress.subnetIP.sin_addr.s_addr = 0xffffffff;

			natAddr = remoteNormal;
		}

		if (addr[n].IsNatIP())
		{
			natAddr = addr[n];
			natAddr.sin_port = node.port;
		}
	}

	if (natAddr.IsNatIP())
	{
		node.clientAddress.subnetIP = natAddr;
	}

	node.digits = digits;
	mgr->insert_Node(digits, &node);
	//
	return send_welcome(digits, node.clientAddress);
}

int RsNpTracker::get_req_res(char* msg, int size)
{
    RSLOGE("NP: REQ_RES ! recieved  NP2TS_REQ_RES MSG |on_NP2TS_REQ_RES.\n");
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsReqRes reqres_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    reqres_msg.parse(&streamer);
	
	map_str digits;
	memcpy(digits.str_, reqres_msg.uuid, UUID_LENGTH);
	digits.str_[UUID_LENGTH] = 0;

	//RESOURCE MD5(MD5_LEN)
	MD5_Hash_Str hash;
	memcpy(hash.hash_, reqres_msg.resource_md5, MD5_LEN);
	hash.hash_[MD5_LEN] = 0;
	return send_peers(digits, hash);
}

int RsNpTracker::get_report(char* msg, int size)
{
    RSLOGE("NP:  REPORT ! recieved  NP2TS_REPORT MSG |on_NP2TS_REPORT.\n");
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsReport report_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    report_msg.parse(&streamer);

	map_str digits;
	memcpy(digits.str_, report_msg.uuid, UUID_LENGTH);
	digits.str_[UUID_LENGTH] = 0;

	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();
	NPNode node;

	if (mgr->get_Node(digits, &node) == -1)
	{
		RSLOGE("not found this %s client in on_NP2TS_REPORT\n", digits.str_);
		send_msg();
		return 0;
	}

	//info of peer(CorePeerInfo)
	memcpy((char*)&(node.coreInfo), (char*)&(report_msg.coreinfo), sizeof(node.coreInfo));

	//Interval count(UINT8)
	node.countInterval = report_msg.interval_count;
	if(node.countInterval == 0)
	{
		RSLOGE("no resource Interval count(UINT8) in on_NP2TS_REPORT\n");
		return 0;
	}

	
	//BlockInterval
	for (int i =0; i < node.countInterval; i++)
	{
		node.intervalArray.AddInterval(report_msg.blockinterval[i].start, report_msg.blockinterval[i].size);
	}

	//Transfer Info(TransferInfo)
	memcpy((char*)&(node.transInfo), (char*)&(report_msg.transferinfo), sizeof(node.transInfo));

	//playing block(UINT32)
	node.playingBlock = report_msg.playing_block;

	//Current Buffering Time(UINT16)
	node.currentBufferingTime = report_msg.current_buffering_time;

	//Buffered Count(UINT16)
	node.bufferedCount =  report_msg.buffered_count;

	//Buffered Time(UINT16)
	node.bufferedTime = report_msg.buffered_time; 
	
	//Connect Fail Count(UINT16)
	node.connectFailCount = report_msg.connect_fail_count;

	//Incoming Connection Count(UINT16)
	node.incomingConnectionCount = report_msg.incoming_connection_count;

	//Outgoing Connection Count(UINT16)
	node.outgoingConnectionCount = report_msg.outgoing_connection_count;

	//Avg Incoming Connection Elapsed Time(UINT16)
	node.AvgincomingConnectionElapsedTime = report_msg.avg_incoming_connection_elapsed_time;

	//Avg OutgoingConnection Elapsed Time(UINT16)
	node.AvgoutgoingConnectionElapsedTime = report_msg.avg_outgoing_connection_elapsed_time;

	//Message Percent(float)
	node.msgPercent = report_msg.message_percent;

	mgr->insert_Node(digits, &node);

	return 0;
}

int RsNpTracker::get_need_peers(char* msg, int size)
{
    RSLOGE("NP: NEED_PEERS ! recieved  NP2TS_NEED_PEERS MSG |on_NP2TS_NEED_PEERS.\n");
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsNeedPeers needpeers_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    needpeers_msg.parse(&streamer);
	//
	map_str digits;
	memcpy(digits.str_, needpeers_msg.uuid, UUID_LENGTH);

	digits.str_[UUID_LENGTH] = 0;

	//current block(UINT32)
	uint32_t currentBlock = needpeers_msg.current_block;

	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();
	NPNode node;
	
	if (mgr->get_Node(digits, &node) == -1)
	{
		RSLOGE("no npnode in on_NP2TS_NEED_PEERS\n");
		return 0;
		
	}

	time(&(node.last_recv_report_time_));

	return send_peers(digits, node.channelID_md5, currentBlock);
}

int RsNpTracker::get_logout(char* msg, int size)
{
    RSLOGE("NP: LOGOUT! recieved  NP2TS_LOGOUT MSG |on_NP2TS_LOGOUT.\n");
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsLogout logout_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    logout_msg.parse(&streamer);

	map_str digits;
	memcpy(digits.str_, logout_msg.uuid, UUID_LENGTH);

	digits.str_[UUID_LENGTH] = 0;

	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();

	mgr->deleteNode(digits);
	
	return 0;
}

int RsNpTracker::get_res_interval(char* msg, int size)
{
    printf("%s\n", __FUNCTION__);
    int ret = ERROR_SUCCESS;
    Np2TsReport res_interval_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    res_interval_msg.parse(&streamer);

	map_str digits;
	//
	memcpy(digits.str_, res_interval_msg.uuid, UUID_LENGTH);

	digits.str_[UUID_LENGTH] = 0;

	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();

	NPNode node;
	if (mgr->get_Node(digits, &node) == -1)
	{
		send_msg();
	}else
	{
		send_res_interval(node.channelID_md5);
	}	
}

int RsNpTracker::send_peers(map_str uuid, MD5_Hash_Str resHash, uint32_t currentblockID)
{
	char* payload = NULL;
    int payload_nb = 0;

	PeerInfoWithAddr* pPeerInfoWithAddr =NULL;
	NetAddress* pCPAddr= NULL;
	NetAddress* pSPAddr= NULL;

	TrackerNpCoordinator* mgr = TrackerNpCoordinator::instance();
	NPNode pNode;

	StreamMgr* chn_mgr = StreamMgr::instance();
    
	uint8_t npCount = mgr->get_np_address(resHash, uuid, pPeerInfoWithAddr, 20, currentblockID);
	uint8_t cpCount = chn_mgr->get_cp_address(pCPAddr, 10);
	uint8_t spCount = chn_mgr->get_sp_address(resHash, pSPAddr, 1);

	if (cpCount == 0 || spCount == 0)
	{
		RSLOGE("no cp or sp in send_TS2NP_PEERS\n");
		return 0;
	}

	Ts2NpPeers peers_msg;
	peers_msg.sp_list_size = cpCount;
	peers_msg.sp_addr = new NetAddress[cpCount];
	peers_msg.peer_list_size = npCount;
	peers_msg.peer_addr = new PeerInfoWithAddr[npCount];
   
	if (mgr->get_Node(uuid, &pNode)==-1)
	{
		RSLOGE("no npnode in send_TS2NP_PEERS\n");
		send_msg();
		goto del;
	}
	//

	time(&(pNode.last_recv_report_time_));

	pNode.channelID_md5 = resHash;	

	for (int cp = 0; cp < cpCount; cp++)
	{
		memcpy((char*)&(peers_msg.sp_addr[cp]), (char*)&pCPAddr[cp], sizeof(pCPAddr[cp]));
	}

	for (int np = 0; np < npCount; np++)
	{
		memcpy((char*)&(peers_msg.peer_addr[np]), (char*)&pPeerInfoWithAddr[np], sizeof(PeerInfoWithAddr));
	}

	memcpy((char*)&peers_msg.np_addr, (char*)&pSPAddr[0], sizeof(NetAddress));

	RSLOGE("send TS2NP_PEERS to %s |send_TS2NP_PEERS::cpCount:%d   spCount:%d  \
									npCount:%d    .\n", uuid.str_, cpCount, spCount, npCount ); 

	mgr->insert_Node(uuid, &pNode);

    peers_msg.pack(payload, payload_nb);
    send_buffer(payload, payload_nb);

del:
	
	if (pPeerInfoWithAddr != NULL)
	{
		delete []pPeerInfoWithAddr;
	}

	if (pCPAddr != NULL)
	{
		delete []pCPAddr;
	}

	if (pSPAddr != NULL)
	{
		delete []pSPAddr;
	}
	
	return 0;	
}

int RsNpTracker::send_welcome(map_str digits, P2PAddress p2pAddr)
{
	int ret = ERROR_SUCCESS;
	Ts2NpWelcome welcome_msg;
	memcpy((char*)&welcome_msg.uuid, digits.str_, UUID_LENGTH);
	memcpy((char*)&welcome_msg.peer_ip, (char*)&p2pAddr, sizeof(P2PAddress));

	RSLOGE("send  TS2NP_WELCOME MSG |send_TS2NP_WELCOME.\n");

	char* payload = NULL;
    int payload_nb = 0;
    welcome_msg.pack(payload, payload_nb);
    send_buffer(payload, payload_nb);
	
	return ret;
}

int RsNpTracker::send_res_interval(MD5_Hash_Str channel_hash)
{
	BlockInterval blockInterval;
	StreamMgr* mgr = StreamMgr::instance();
	if (1 == mgr->get_channel_interval(channel_hash,  blockInterval))
	{
		RSLOGE("no blockInterval in send_TS2NP_RES_INTERVAL\n");
		return 0;
	}

	//signal to get res interval if blockinterval is not updated since last time
	if(last_send_blockinterval == blockInterval)
	{
		mgr->signal_get_res_interval(channel_hash);
		last_send_blockinterval = blockInterval;
		//ACE_DEBUG((LM_ERROR, ACE_TEXT("blockInterval is not updated in send_TS2NP_RES_INTERVAL\n")));
		return 0;
	}

	Ts2NpResInterval res_interval_msg;
	memcpy((char*)&res_interval_msg.block_interval, (char*)&blockInterval, sizeof(BlockInterval));

	last_send_blockinterval = blockInterval;

	char* payload = NULL;
    int payload_nb = 0;
    res_interval_msg.pack(payload, payload_nb);
    send_buffer(payload, payload_nb);
}

void RsNpTracker::send_msg()
{
	Ts2NpMsg msg;

	bool bFlag = true;
	msg.err_msg_type = bFlag;
	RSLOGE("send  TS2NP_MSG MSG |send_TS2NP_MSG.\n");

	char* payload = NULL;
    int payload_nb = 0;
    msg.pack(payload, payload_nb);
    send_buffer(payload, payload_nb);
}

int RsNpTracker::loop()
{
    int ret = ERROR_SUCCESS;
	return ret;
}

int RsNpTracker::handle_udp_packet(st_netfd_t st_fd, sockaddr_in* from, char* buf, int nb_buf)
{
    int ret = ERROR_SUCCESS;

	if(buf==NULL || nb_buf==0)
		return -1;//fixme, error code
    
	sp_fd = st_fd;
	//save the remote address
	last_receive_addr = *from;
    //get message size
    char* temp = buf;
    int msg_size = 0;//(temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0];
    memcpy((char*)&msg_size, buf, 4);
    buf += 4;

    //get the message type
    char  msg_type = 0;
    msg_type = *buf;
    buf += 1;

    RSLOGE("%s type:%d size:%d\n", __FUNCTION__, msg_type, msg_size);

    switch(msg_type)
    {
		case NP2TS_LOGIN:
			get_login(temp+5, msg_size-5);
			break;
		case NP2TS_REQ_RES:
			get_req_res(temp+5, msg_size-5);
			break;
		case NP2TS_REPORT:
			get_report(temp+5, msg_size-5);
			break;
		case NP2TS_NEED_PEERS:
			get_need_peers(temp+5, msg_size-5);
			break;
		case NP2TS_RES_INTERVAL:
			get_res_interval(temp+5, msg_size-5);
			break;
		case NP2TS_LOGOUT:
			get_logout(temp+5, msg_size-5);
			break;
    }

    if (UDP_PACKET_RECV_CYCLE_INTERVAL_MS > 0) {
        st_usleep(UDP_PACKET_RECV_CYCLE_INTERVAL_MS * 1000);
    }
    return ret;
}

int RsNpTracker::handle_timeout(int64_t timerid)
{
    int ret = ERROR_SUCCESS;
    switch(timerid)
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

int RsNpTracker::send_buffer(char* buf, int size)
{
    int ret = ERROR_SUCCESS;
	ssize_t nsize = 0;
    if((buf!=NULL)&&(size!=0))
        nsize = st_sendto(sp_fd, buf, size, (sockaddr*)&last_receive_addr, sizeof(last_receive_addr), ST_UTIME_NO_TIMEOUT);

	if(nsize != size)
		return -1;//todo, Kevin, handle error code
	return ret;
}
