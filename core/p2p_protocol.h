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

#ifndef CORE_RS_P2P_PROTOCOL_H__
#define CORE_RS_P2P_PROTOCOL_H__

namespace rs {
namespace core {

// Capture Server = CS; Super Peer = SP; Tracker Server = TS; Normal Peer = NP;
// SP<------SPandTS(UDP)------>TS
// NP<------NPandTS(UDP)------>TS
// CS<------CSandSP(TCP)------>SP
// SP<------SPandSP(TCP)------>SP
// SP<------SPandSP(TCP)------>NP
// NP<------NPandNP(TCP)------>NP

// message format
// |msg size(UINT32)|msg type(BYTE)|msg content(...)|
// |<-----msg size is the total size of message---->|

enum P2PMESSAGE {
    // CS<---->SP
    // |size of channel name(UINT8)|channel name(...)|
    // |userid(UINT32)|md5 password(MD5_LEN)|
    // |max blocksize(UINT32)|max filesize(UINT32)|channel bitRate(float)|
    // |isSource(bool)|size of CHANNEL DATA(UINT32)|CHANNEL DATA(...)|
    CS2SP_REGISTER = 0x10,
    // |channel bitRate(float)|
    CS2SP_UPDATE = 0x11,
    // |block id(UINT32)|block size(UINT32)|
    // |block data(...)|
    CS2SP_BLOCK = 0x12,
    // |media data length(UINT32)|media data(...)|
    CS2SP_MEDIA_TYPE = 0x13,

    // |start block ID(UINT32)|
    SP2CS_WELCOME = 0x14,
    // |err msg type(UINT16)|should quit(bool)|
    SP2CS_MSG = 0x15,

    // TS<---->SP
    // |userID(UINT32)|md5 password(MD5_LEN)|service port(USHORT)|
    SP2TS_REGISTER = 0x20,
    // |UUID(16 bytes)|
    // |resource number(UINT32)|connection number(UINT16)|bandwidth
    // usage(float)| |exceed max connection(bool)|
    SP2TS_STATUS = 0x21,
    // |UUID(16 bytes)|
    // |resource count(UINT32)|first resource MD5(MD5_LEN)|first resource block
    // interval(BlockInterval)...|
    SP2TS_RES_LIST = 0x22,
    // |UUID(16 bytes)|
    SP2TS_GET_SP = 0x23,
    // |UUID(16 bytes)|
    SP2TS_LOGOUT = 0x24,

    // |UUID(16 bytes)|
    TS2SP_WELCOME = 0x25,
    // |err msg type(UINT16)|should quit(bool)|
    TS2SP_MSG = 0x26,
    //|count(UINT8)|first spaddress(NormalAddress)...|
    TS2SP_SP_LIST = 0x27,

    TS2SP_GET_RES_LIST = 0x28,

    // TS<---->NP
    // |login id(UINT32)|md5 password(MD5_LEN)|
    // |version of client(float)|listening port(USHORT)|
    // |size of local ip list(UINT8)|first ip addr(in_addr)|...|
    NP2TS_LOGIN = 0x30,
    // |UUID(16 BYTEs)|
    // |RESOURCE MD5(MD5_LEN)|
    NP2TS_REQ_RES = 0x31,
    // |UUID(16 BYTEs)|
    // |info of peer(CorePeerInfo)|
    // |Interval count(UINT8)|first BlockInterval|...|
    // |Transfer Info(TransferInfo)|
    // |playing block(UINT32)|Current Buffering Time(UINT16)|Buffered
    // Count(UINT16)|Buffered Time(UINT16)| |Connect Fail Count(UINT16)|Incoming
    // Connection Count(UINT16)|Outgoing Connection Count(UINT16)| |Avg Incoming
    // Connection Elapsed Time(UINT16)|Avg OutgoingConnection Elapsed
    // Time(UINT16)| |Message Percent(float)|
    NP2TS_REPORT = 0x32,
    // |UUID(16 BYTEs)|current block(UINT32)|
    NP2TS_NEED_PEERS = 0x33,
    // |UUID(16 BYTEs)|
    NP2TS_RES_INTERVAL = 0x34,
    // |UUID(16 BYTEs)|
    NP2TS_LOGOUT = 0x35,

    // |UUID(16 BYTEs)|ip of peer(P2PAddress)|
    TS2NP_WELCOME = 0x36,
    // |SP list size(UINT8)|first SP addr(NormalAddress)|..|
    // |peer list size(UINT8)|peer1(PeerInfoWithAddr)|...|SP
    // addr(NormalAddress)|
    TS2NP_PEERS = 0x37,
    // |resource block interval(BlockInterval)|
    TS2NP_RES_INTERVAL = 0x38,
    // |err msg type(UINT16)|should quit(bool)|
    TS2NP_MSG = 0x39,

    // NP<---->NP
    // |NP version(float)|
    // |RESOURCE MD5(MD5_LEN)|Passive Connection(bool)|
    // |info of peer(PeerInfoWithAddr)|
    NP2NP_HELLO = 0x40,
    // |info of peer(CorePeerInfo)|refresh(bool)|
    // |Interval count(UINT8)|first BlockInterval|...|
    NP2NP_REPORT = 0x41,
    // |peer list size(UINT8)|first peer(PeerInfoWithAddr)|...|
    NP2NP_NEAR_PEERS = 0x42,
    // |block count(UINT8)|first blockid(UINT32)|...|
    NP2NP_PUSHLIST = 0x43,
    // |block ID(UINT32))|block size(UINT32)|block data(...)|
    NP2NP_RESPONSE = 0x44,
    // |startid(UINT32)|length(UINT32)|media data length(UINT32)|media
    // data(...)| |program name size(UINT8)|program name(...)|program time in
    // seconds(UINT32)|channel name size(UINT8)|channel name(...)|
    NP2NP_MEDIATYPE = 0x45,
    // |err msg type(UINT16)|should quit(bool)|
    NP2NP_MSG = 0x46,

    // SP<---->SP
    // |RESOURCE MD5(MD5_LEN)|source superpeer address(NormalAddress)|
    // |block count(UINT8)|first blockid(UINT32)|...|
    SP2SP_PUSHLIST = 0x50,
    // |RESOURCE MD5(MD5_LEN)|
    // |block ID(UINT32))|block size(UINT32)|block data(...)|
    SP2SP_RESPONSE = 0x51,
    // |RESOURCE MD5(MD5_LEN)|
    // |startid(UINT32)|length(UINT32)|media data length(UINT32)|media
    // data(...)| |program name size(UINT8)|program name(...)|program time in
    // seconds(UINT32)|channel name size(UINT8)|channel name(...)|
    SP2SP_MEDIATYPE = 0x52,
    // |RESOURCE MD5(MD5_LEN)|
    // |err msg type(UINT16)|should quit(bool)|
    SP2SP_MSG = 0x53,
};

} // namespace core
} // namespace rs

#endif
