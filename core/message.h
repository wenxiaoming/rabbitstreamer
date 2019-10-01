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
#ifndef CORE_RS_MESSAGE_H_
#define CORE_RS_MESSAGE_H_

#include <stdint.h>
#include "core_struct.h"
#include "streamer.h"

namespace rs {
namespace core {

class RsMessage {
public:
    RsMessage();
    virtual ~RsMessage();
public:
    virtual int parse(RsStreamer* streamer);
    virtual int pack(char*& payload, int& nb);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
    char* buffer;
    uint32_t msg_size;
    uint8_t msg_type;
};

//SP2TS_REGISTER
class Sp2TsRegister:public RsMessage {
public:
    Sp2TsRegister();
    ~Sp2TsRegister();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    uint32_t user_id;
    char password[MD5_LEN];
    uint16_t service_port;
};

//TS2SP_WELCOME
class Ts2SpWelcome:public RsMessage {
public:
    Ts2SpWelcome();
    ~Ts2SpWelcome();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char sp_uuid[UUID_LENGTH];
};

//TS2SP_SP_LIST
class Ts2SpSpList:public RsMessage {
public:
    Ts2SpSpList();
    ~Ts2SpSpList();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    uint8_t count;
    NetAddress* addr;
};

//SP2TS_GET_SP
class Sp2TsSpList:public RsMessage {
public:
    Sp2TsSpList();
    ~Sp2TsSpList();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char sp_uuid[UUID_LENGTH];
};

typedef struct resource_info {
    char res_md5[MD5_LEN];
    BlockInterval block_interval;
} resource_info;

//SP2TS_RES_LIST
class Sp2TsResList:public RsMessage {
public:
    Sp2TsResList();
    ~Sp2TsResList();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char sp_uuid[UUID_LENGTH];
    uint32_t resource_count;
    resource_info* res_info;
};

class Sp2TsStatus:public RsMessage {
public:
    Sp2TsStatus();
    ~Sp2TsStatus();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char sp_uuid[UUID_LENGTH];
    uint32_t resource_count;
    uint16_t connection_num;
    float bandwidth;
    uint8_t exceed_max_connection;
};

// |UUID(16 bytes)|
//SP2TS_LOGOUT      = 0x24,
class Sp2TsLogout:public RsMessage {
public:
    Sp2TsLogout();
    ~Sp2TsLogout();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char sp_uuid[UUID_LENGTH];
};

//NP2TS_LOGIN       = 0x30,
class Np2TsLogin:public RsMessage {
public:
    Np2TsLogin();
    ~Np2TsLogin();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    uint32_t login_id;
    char password[MD5_LEN];
    float client_version;
    uint16_t listening_port;
    uint8_t local_ip_list_size;
    in_addr* ip_addr;
};

// NP2TS_REQ_RES     = 0x31,
class Np2TsReqRes:public RsMessage {
public:
    Np2TsReqRes();
    ~Np2TsReqRes();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
    char resource_md5[MD5_LEN];
};

//NP2TS_REPORT      = 0x32,
class Np2TsReport:public RsMessage {
public:
    Np2TsReport();
    ~Np2TsReport();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
    CorePeerInfo coreinfo;
    uint8_t interval_count;
    BlockInterval* blockinterval;
    TransferInfo transferinfo;
    uint32_t playing_block;
    uint16_t current_buffering_time;
    uint16_t buffered_count;
    uint16_t buffered_time;
    uint16_t connect_fail_count;
    uint16_t incoming_connection_count;
    uint16_t outgoing_connection_count;
    uint16_t avg_incoming_connection_elapsed_time;
    uint16_t avg_outgoing_connection_elapsed_time;
    float message_percent;
};

//NP2TS_NEED_PEERS  = 0x33,
class Np2TsNeedPeers:public RsMessage {
public:
    Np2TsNeedPeers();
    ~Np2TsNeedPeers();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
    uint32_t current_block;
};

//NP2TS_RES_INTERVAL= 0x34,
class Np2TsReqInterval:public RsMessage {
public:
    Np2TsReqInterval();
    ~Np2TsReqInterval();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
};

//NP2TS_LOGOUT      = 0x35,
class Np2TsLogout:public RsMessage {
public:
    Np2TsLogout();
    ~Np2TsLogout();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
};

//TS2NP_WELCOME     = 0x36,
class Ts2NpWelcome:public RsMessage {
public:
    Ts2NpWelcome();
    ~Ts2NpWelcome();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    char uuid[UUID_LENGTH];
    P2PAddress peer_ip;
};

//TS2NP_PEERS       = 0x37,
class Ts2NpPeers:public RsMessage {
public:
    Ts2NpPeers();
    ~Ts2NpPeers();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    uint8_t sp_list_size;
    NetAddress* sp_addr;
    uint8_t peer_list_size;
    PeerInfoWithAddr* peer_addr;
    NetAddress np_addr;//Kevin, fix me
};

//TS2NP_RES_INTERVAL= 0x38,
class Ts2NpResInterval:public RsMessage {
public:
    Ts2NpResInterval();
    ~Ts2NpResInterval();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    BlockInterval block_interval;
};

//TS2NP_MSG         = 0x39,
class Ts2NpMsg:public RsMessage {
public:
    Ts2NpMsg();
    ~Ts2NpMsg();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    uint16_t err_msg_type;
    bool should_quit;
};

//SP2SP_PUSHLIST
class Sp2SpPushList:public RsMessage {
public:
    Sp2SpPushList();
    ~Sp2SpPushList();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char resource_md5[MD5_LEN];
    NetAddress source_sp_addr;
    uint8_t block_count;
    uint32_t* block_array;
};

//SP2SP_MEDIATYPE
class Sp2SpMediatype:public RsMessage {
public:
    Sp2SpMediatype();
    ~Sp2SpMediatype();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char resource_md5[MD5_LEN];
    uint32_t block_start_id;
    uint32_t block_num;
    uint32_t media_type_size;
    char* media_type;
    uint8_t program_name_size;
    char* program_name;
    uint32_t program_time;
    uint8_t channel_name_size;
    char* channel_name;
};

//SP2SP_RESPONSE
class Sp2SpResponse:public RsMessage {
public:
    Sp2SpResponse();
    ~Sp2SpResponse();
public:
    virtual int parse(RsStreamer* streamer);
protected:
    virtual int pack_submsg(RsStreamer* streamer);
    virtual int reserve_pack_size();
public:
    //uuid of superpeer on tracker
    char resource_md5[MD5_LEN];
    uint32_t block_id;
    uint32_t block_size;
    char* block_data;
};

} // namespace rs::core

#endif /* CORE_RS_MESSAGE_H_ */
