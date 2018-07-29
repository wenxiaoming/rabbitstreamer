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

#include "message.h"
#include "p2p_protocol.h"
#include "error_code.h"

RsMessage::RsMessage()
{
    msg_size = 0;
    msg_type = 0;
    buffer = NULL;
}

RsMessage::~RsMessage()
{
    //if(buffer) /////FIXME:release it?
    //    delete[] buffer;
}

int RsMessage::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int RsMessage::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int RsMessage::reserve_pack_size()
{
    return sizeof(msg_size)+sizeof(msg_type);
}

int RsMessage::pack(char*& payload, int& nb)
{
    int ret = ERROR_SUCCESS;
    int size = reserve_pack_size();
    buffer = new char[size];//FIXME: to free this buffer somewhere
    if(!buffer)
        return ERROR_SYSTEM_FAIL_TO_ALLOCATE;
    RsStreamer streamer;
    streamer.initialize(buffer, size);
    streamer.write_int(size);
    pack_submsg(&streamer);
    payload = streamer.get_buffer();
    nb = streamer.get_size();
    return ret;
}

Sp2TsRegister::Sp2TsRegister()
{
    user_id = 0x04030201;//just for test
    memset(password, 0, MD5_LEN);
    service_port = 0;
}

Sp2TsRegister::~Sp2TsRegister()
{

}

int Sp2TsRegister::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    user_id = streamer->read_int();
    memcpy(password, streamer->read_nbytes(MD5_LEN), MD5_LEN);
    service_port = streamer->read_short();
    return ret;
}

int Sp2TsRegister::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(user_id) + sizeof(password)+sizeof(service_port);
    return total_size;
}

int Sp2TsRegister::pack_submsg(RsStreamer* streamer)
{
    streamer->write_char(SP2TS_REGISTER);
    streamer->write_int(user_id);
    streamer->write_nbytes(password, MD5_LEN);
    streamer->write_short(service_port);
}

//Ts2SpWelcome
Ts2SpWelcome::Ts2SpWelcome()
{
}

Ts2SpWelcome::~Ts2SpWelcome()
{
}

int Ts2SpWelcome::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(sp_uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    return ret;
}

int Ts2SpWelcome::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_uuid);
    return total_size;
}

int Ts2SpWelcome::pack_submsg(RsStreamer* streamer)
{
    streamer->write_char(TS2SP_WELCOME);
    streamer->write_nbytes(sp_uuid, UUID_LENGTH);
}

//TS2SP_SP_LIST
Ts2SpSpList::Ts2SpSpList()
{
}

Ts2SpSpList::~Ts2SpSpList()
{
    //if(addr) //todo, confirm it?
    //    delete[] addr;
}

int Ts2SpSpList::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    count = streamer->read_char();

    if(count==0)
        return 0;//todo, return error code

    addr = new NetAddress[count];
    for(int i = 0; i < count; i++) {
        memcpy((char*)&addr[i], streamer->read_nbytes(sizeof(NetAddress)), sizeof(NetAddress));
    }
    return ret;
}

int Ts2SpSpList::pack_submsg(RsStreamer* streamer)
{
    streamer->write_char(TS2SP_SP_LIST);
    streamer->write_char(count);
    for(int i = 0; i < count; i++) {
        streamer->write_nbytes((char*)&addr[i], sizeof(NetAddress));
    }
}

int Ts2SpSpList::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(count);
    total_size += count*sizeof(NetAddress);
    return total_size;
}

//Sp2TsSpList
Sp2TsSpList::Sp2TsSpList()
{
}

Sp2TsSpList::~Sp2TsSpList()
{
}

int Sp2TsSpList::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(sp_uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    return ret;
}

int Sp2TsSpList::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_uuid);
    return total_size;
}

int Sp2TsSpList::pack_submsg(RsStreamer* streamer)
{
    streamer->write_char(SP2TS_GET_SP);
    streamer->write_nbytes(sp_uuid, UUID_LENGTH);
}

//Sp2TsLogout
Sp2TsLogout::Sp2TsLogout()
{
}

Sp2TsLogout::~Sp2TsLogout()
{
}

int Sp2TsLogout::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(sp_uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    return ret;
}

int Sp2TsLogout::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_uuid);
    return total_size;
}

int Sp2TsLogout::pack_submsg(RsStreamer* streamer)
{
    streamer->write_char(SP2TS_GET_SP);
    streamer->write_nbytes(sp_uuid, UUID_LENGTH);
}

//Sp2TsResList
Sp2TsResList::Sp2TsResList()
{
    resource_count = 0;
    res_info = NULL;
}

Sp2TsResList::~Sp2TsResList()
{
}

int Sp2TsResList::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(sp_uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    resource_count = streamer->read_int();
    for(int i = 0; i < resource_count; i++) {
        memcpy(res_info[i].res_md5, streamer->read_nbytes(MD5_LEN), MD5_LEN);
        res_info[i].block_interval.start = streamer->read_int();
        res_info[i].block_interval.size = streamer->read_int();
    }
    return ret;
}

int Sp2TsResList::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_uuid);
    total_size += sizeof(resource_count);
    total_size += resource_count*sizeof(resource_info);
    return total_size;
}

int Sp2TsResList::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(SP2TS_RES_LIST);
    streamer->write_nbytes(sp_uuid, UUID_LENGTH);
    streamer->write_int(resource_count);
    for(int i = 0; i < resource_count; i++) {
        streamer->write_nbytes(res_info[i].res_md5, MD5_LEN);
        streamer->write_int(res_info[i].block_interval.start);
        streamer->write_int(res_info[i].block_interval.size);
    }
    return ret;
}

Sp2TsStatus::Sp2TsStatus()
{
}

Sp2TsStatus::~Sp2TsStatus()
{
}

int Sp2TsStatus::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(sp_uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    resource_count = streamer->read_int();
    connection_num = streamer->read_short();
    memcpy(&bandwidth, streamer->read_nbytes(sizeof(bandwidth)), sizeof(bandwidth));
    exceed_max_connection = streamer->read_char();
    return ret;
}

int Sp2TsStatus::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_uuid);
    total_size += sizeof(resource_count);
    total_size += sizeof(connection_num);
    total_size += sizeof(bandwidth);
    total_size += sizeof(exceed_max_connection);
    return total_size;
}

int Sp2TsStatus::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(SP2TS_STATUS);
    streamer->write_nbytes(sp_uuid, UUID_LENGTH);
    streamer->write_int(resource_count);
    streamer->write_short(connection_num);
    streamer->write_nbytes((char*)&bandwidth, sizeof(bandwidth));
    streamer->write_char(exceed_max_connection);
    return ret;
}

//Sp2SpPushList
Sp2SpPushList::Sp2SpPushList()
{
    block_count = 0;
    block_array = NULL;
    memset(&source_sp_addr, 0, sizeof(source_sp_addr));
    memset(resource_md5, 0, MD5_LEN);
}

Sp2SpPushList::~Sp2SpPushList()
{
    if(block_array)
        delete[] block_array;
}

int Sp2SpPushList::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(resource_md5, streamer->read_nbytes(MD5_LEN), MD5_LEN);
    memcpy(&source_sp_addr, streamer->read_nbytes(sizeof(source_sp_addr)), sizeof(source_sp_addr));
    block_count = streamer->read_char();
    if(block_count) {
        block_array = new uint32_t[block_count];
        for(int i = 0; i < block_count; i++) {
            block_array[i] = streamer->read_int();
        }
    }
    return ret;
}

int Sp2SpPushList::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(block_count);
    total_size += sizeof(block_array);
    total_size += block_count*sizeof(uint32_t);
    return total_size;
}

int Sp2SpPushList::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    return ret;
}

//Sp2SpMediatype
Sp2SpMediatype::Sp2SpMediatype()
{

}

Sp2SpMediatype::~Sp2SpMediatype()
{

}

int Sp2SpMediatype::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int Sp2SpMediatype::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(resource_md5);
    total_size += sizeof(block_start_id);
    total_size += sizeof(block_num);
    total_size += sizeof(media_type_size);
    total_size += sizeof(char)*media_type_size;
    total_size += sizeof(program_name_size);
    total_size += sizeof(char)*program_name_size;
    total_size += sizeof(program_time);
    total_size += sizeof(channel_name_size);
    total_size += sizeof(char)*channel_name_size;
    return total_size;
}

int Sp2SpMediatype::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(SP2SP_MEDIATYPE);
    streamer->write_nbytes(resource_md5, MD5_LEN);
    streamer->write_int(block_start_id);
    streamer->write_int(block_num);

    streamer->write_int(media_type_size);
    if(media_type_size)
        streamer->write_nbytes(media_type, media_type_size);

    streamer->write_char(program_name_size);
    if(program_name_size)
        streamer->write_nbytes(program_name, program_name_size);

    streamer->write_int(program_time);

    streamer->write_char(channel_name_size);
    if(channel_name_size)
        streamer->write_nbytes(channel_name, channel_name_size);

    return ret;
}

//Sp2SpResponse
Sp2SpResponse::Sp2SpResponse()
{
}

Sp2SpResponse::~Sp2SpResponse()
{
}

int Sp2SpResponse::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int Sp2SpResponse::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(resource_md5);
    total_size += sizeof(block_id);
    total_size += sizeof(block_size);
    total_size += block_size*sizeof(char);
    return total_size;
}

int Sp2SpResponse::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(SP2SP_RESPONSE);
    streamer->write_nbytes(resource_md5, MD5_LEN);
    streamer->write_int(block_id);
    streamer->write_int(block_size);
    if(block_size)
        streamer->write_nbytes(block_data, block_size);

    return ret;
}

//NP2TS_LOGIN       = 0x30,
Np2TsLogin::Np2TsLogin()
{
    ip_addr = NULL;
}

Np2TsLogin::~Np2TsLogin()
{
    if(ip_addr)
        delete[] ip_addr;
}

int Np2TsLogin::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    login_id = streamer->read_int();
    memcpy(password, streamer->read_nbytes(MD5_LEN), MD5_LEN);
    memcpy((char*)&client_version, streamer->read_nbytes(sizeof(float)), sizeof(float));
    listening_port = streamer->read_short();
    local_ip_list_size = streamer->read_char();
    if(local_ip_list_size)
        ip_addr = new in_addr[local_ip_list_size];

    for(int i = 0; i < local_ip_list_size; i++)
        memcpy((char*)&(ip_addr[i]), streamer->read_nbytes(sizeof(in_addr)), sizeof(in_addr));

    return ret;
}

int Np2TsLogin::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_LOGIN);
    streamer->write_int(login_id);
    streamer->write_nbytes(password, MD5_LEN);
    streamer->write_nbytes((char*)&client_version, sizeof(client_version));
    streamer->write_short(listening_port);
    streamer->write_char(local_ip_list_size);

    for(int i = 0; i < local_ip_list_size; i++)
        streamer->write_nbytes((char*)&(ip_addr[i]), sizeof(in_addr));

    return ret;
}

int Np2TsLogin::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(login_id);
    total_size += MD5_LEN;
    total_size += sizeof(client_version);
    total_size += sizeof(listening_port);
    total_size += sizeof(local_ip_list_size);
    total_size += local_ip_list_size*sizeof(in_addr);
    return total_size;
}

// |UUID(16 BYTEs)|
// |RESOURCE MD5(MD5_LEN)|
// NP2TS_REQ_RES     = 0x31,
Np2TsReqRes::Np2TsReqRes()
{

}

Np2TsReqRes::~Np2TsReqRes()
{

}

int Np2TsReqRes::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    memcpy(resource_md5, streamer->read_nbytes(MD5_LEN), MD5_LEN);
    return ret;
}

int Np2TsReqRes::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_REQ_RES);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    streamer->write_nbytes(resource_md5, MD5_LEN);
    return ret;
}

int Np2TsReqRes::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;
    total_size += MD5_LEN;
    return total_size;
}


//NP2TS_REPORT      = 0x32,
Np2TsReport::Np2TsReport()
{

}

Np2TsReport::~Np2TsReport()
{

}

int Np2TsReport::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);

    memcpy((char*)&coreinfo, streamer->read_nbytes(sizeof(CorePeerInfo)), sizeof(CorePeerInfo));

    interval_count = streamer->read_char();
    for(int i = 0; i < interval_count; i++)
        memcpy((char*)&blockinterval[i], streamer->read_nbytes(sizeof(BlockInterval)), sizeof(BlockInterval));

    memcpy((char*)&transferinfo, streamer->read_nbytes(sizeof(transferinfo)), sizeof(transferinfo));
    playing_block = streamer->read_int();
    current_buffering_time = streamer->read_short();
    buffered_count = streamer->read_short();
    buffered_time = streamer->read_short();
    connect_fail_count = streamer->read_short();
    incoming_connection_count = streamer->read_short();
    outgoing_connection_count = streamer->read_short();
    avg_incoming_connection_elapsed_time = streamer->read_short();
    avg_outgoing_connection_elapsed_time = streamer->read_short();
    memcpy((char*)&message_percent, streamer->read_nbytes(sizeof(message_percent)), sizeof(message_percent));
    return ret;
}

int Np2TsReport::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_REPORT);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    streamer->write_nbytes((char*)&coreinfo, sizeof(CorePeerInfo));

    streamer->write_char(interval_count);
    for(int i = 0; i < interval_count; i++)
        streamer->write_nbytes((char*)&blockinterval[i], sizeof(BlockInterval));

    streamer->write_nbytes((char*)&transferinfo, sizeof(transferinfo));
    streamer->write_int(playing_block);
    streamer->write_short(current_buffering_time);
    streamer->write_short(buffered_count);
    streamer->write_short(buffered_time);
    streamer->write_short(connect_fail_count);
    streamer->write_short(incoming_connection_count);
    streamer->write_short(outgoing_connection_count);
    streamer->write_short(avg_incoming_connection_elapsed_time);
    streamer->write_short(avg_outgoing_connection_elapsed_time);

    return ret;
}

int Np2TsReport::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;

    total_size += sizeof(interval_count);
    for(int i = 0; i < interval_count; i++)
        total_size += sizeof(BlockInterval);

    total_size += sizeof(transferinfo);
    total_size += sizeof(playing_block);
    total_size += sizeof(current_buffering_time);
    total_size += sizeof(buffered_count);
    total_size += sizeof(buffered_time);
    total_size += sizeof(connect_fail_count);
    total_size += sizeof(incoming_connection_count);
    total_size += sizeof(outgoing_connection_count);
    total_size += sizeof(avg_incoming_connection_elapsed_time);
    total_size += sizeof(avg_outgoing_connection_elapsed_time);
    return total_size;
}

//NP2TS_NEED_PEERS  = 0x33,
Np2TsNeedPeers::Np2TsNeedPeers()
{
}


Np2TsNeedPeers::~Np2TsNeedPeers()
{
}

int Np2TsNeedPeers::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    current_block = streamer->read_int();
    return ret;
}

int Np2TsNeedPeers::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_NEED_PEERS);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    streamer->write_int(current_block);
    return ret;
}

int Np2TsNeedPeers::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;
    total_size += sizeof(current_block);
    return total_size;
}

//NP2TS_RES_INTERVAL= 0x34,
Np2TsReqInterval::Np2TsReqInterval()
{
}

Np2TsReqInterval::~Np2TsReqInterval()
{
}

int Np2TsReqInterval::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    return ret;
}

int Np2TsReqInterval::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_RES_INTERVAL);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    return ret;
}

int Np2TsReqInterval::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;
    return total_size;
}

//NP2TS_LOGOUT      = 0x35,
Np2TsLogout::Np2TsLogout()
{
}

Np2TsLogout::~Np2TsLogout()
{
}

int Np2TsLogout::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    return ret;
}

int Np2TsLogout::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_LOGOUT);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    return ret;
}

int Np2TsLogout::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;
    return total_size;
}

Ts2NpWelcome::Ts2NpWelcome()
{

}

Ts2NpWelcome::~Ts2NpWelcome()
{

}

int Ts2NpWelcome::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy(uuid, streamer->read_nbytes(UUID_LENGTH), UUID_LENGTH);
    memcpy((char*)&peer_ip, streamer->read_nbytes(sizeof(P2PAddress)), sizeof(P2PAddress));
    return ret;
}

int Ts2NpWelcome::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(NP2TS_LOGOUT);
    streamer->write_nbytes(uuid, UUID_LENGTH);
    streamer->write_nbytes((char*)&peer_ip, sizeof(P2PAddress));
    return ret;
}

int Ts2NpWelcome::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += UUID_LENGTH;
    total_size += sizeof(P2PAddress);
    return total_size;
}

//TS2NP_PEERS       = 0x37,
Ts2NpPeers::Ts2NpPeers()
{
    sp_addr = NULL;
    peer_addr = NULL;
}

Ts2NpPeers::~Ts2NpPeers()
{
    if(sp_addr)
        delete[] sp_addr;

    if(peer_addr)
        delete[] peer_addr;

}

int Ts2NpPeers::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    sp_list_size = streamer->read_char();

    if(sp_list_size != 0)
        sp_addr = new NetAddress[sp_list_size];

    for(int i = 0; i < sp_list_size; i++)
        memcpy((char*)&sp_addr[i], streamer->read_nbytes(sizeof(NetAddress)), sizeof(NetAddress));

    peer_list_size = streamer->read_char();

    if(peer_list_size)
        peer_addr = new PeerInfoWithAddr[peer_list_size];

    for(int j = 0; j < peer_list_size; j++)
        memcpy((char*)&peer_addr[j], streamer->read_nbytes(sizeof(PeerInfoWithAddr)), sizeof(PeerInfoWithAddr));

    memcpy((char*)&np_addr, streamer->read_nbytes(sizeof(NetAddress)), sizeof(NetAddress));
    return ret;
}

int Ts2NpPeers::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(TS2NP_PEERS);

    streamer->write_char(sp_list_size);
    for(int i = 0; i < sp_list_size; i++)
        streamer->write_nbytes((char*)&sp_addr[i], sizeof(sp_addr[i]));

    streamer->write_char(peer_list_size);
    for(int j = 0; j < sp_list_size; j++)
        streamer->write_nbytes((char*)&peer_addr[j], sizeof(peer_addr[j]));

    streamer->write_nbytes((char*)&np_addr, sizeof(np_addr));
    return ret;
}

int Ts2NpPeers::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(sp_list_size);
    for(int i = 0; i < sp_list_size; i++)
        total_size += sizeof(sizeof(NetAddress));

    total_size += sizeof(peer_list_size);
    for(int j = 0; j < peer_list_size; j++)
        total_size += sizeof(PeerInfoWithAddr);

    total_size += sizeof(np_addr);
    return total_size;
}

//TS2NP_RES_INTERVAL= 0x38,
Ts2NpResInterval::Ts2NpResInterval()
{
}

Ts2NpResInterval::~Ts2NpResInterval()
{
}

int Ts2NpResInterval::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    memcpy((char*)&block_interval, streamer->read_nbytes(sizeof(block_interval)), sizeof(block_interval));
    return ret;
}

int Ts2NpResInterval::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(TS2NP_RES_INTERVAL);
    streamer->write_nbytes((char*)&block_interval, sizeof(block_interval));
    return ret;
}

int Ts2NpResInterval::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(BlockInterval);
    return total_size;
}

//TS2NP_MSG         = 0x39,
Ts2NpMsg::Ts2NpMsg()
{
}

Ts2NpMsg::~Ts2NpMsg()
{
}

int Ts2NpMsg::parse(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    err_msg_type = streamer->read_short();
    should_quit = streamer->read_char();
    return ret;
}

int Ts2NpMsg::pack_submsg(RsStreamer* streamer)
{
    int ret = ERROR_SUCCESS;
    streamer->write_char(TS2NP_MSG);
    streamer->write_short(err_msg_type);
    streamer->write_char(should_quit);
    return ret;
}

int Ts2NpMsg::reserve_pack_size()
{
    int total_size = RsMessage::reserve_pack_size();
    total_size += sizeof(err_msg_type);
    total_size += sizeof(should_quit);
    return total_size;
}
