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
#ifndef RS_CORE_STRUCT_H
#define RS_CORE_STRUCT_H

#include <string.h>
#include <stdint.h>
#include <algorithm>
#include <netinet/in.h>

#define MD5_LEN	 32		// MD5值的长度

using namespace std;

namespace rs {
namespace core {

class NetAddress {
public:
    NetAddress() {
        memset(this, 0, sizeof(NetAddress));
        sin_family = AF_INET;
    }
    NetAddress& operator =(const NetAddress& addr) {
        memcpy(this, &addr, sizeof(NetAddress));
        return *this;
    }

    bool operator == (const NetAddress& addr) const {
        return ((this->sin_addr.s_addr == addr.sin_addr.s_addr) &&
                (this->sin_port == addr.sin_port));
    }

    bool operator != (const NetAddress& addr) const {
        return !(*this == addr);
    }
	
	bool IsNatIP() {
		unsigned long wan_ip;
		unsigned char temp;
		//
		memcpy(&wan_ip, &(sin_addr.s_addr), sizeof(wan_ip));
		/*
		* all nat addresses
		* A 10.0.0.0---10.255.255.255 　　　　1 
		* B 172.16.0.0---172.31.255.255 　　　16 
		* C 192.168.0.0---192.168.255.255 　　255 
		*/
		bool nat = false;
		memcpy(&temp, &wan_ip, 1);
		if(temp == 10) // A 
			nat = true;
		else if(temp == 192) {
			memcpy(&temp, (char*)(&wan_ip)+1, 1);
			if(temp == 168) // B 
				nat = true;
		}
		else if(temp == 172) {
			memcpy(&temp, (char*)(&wan_ip)+1, 1);
			if(temp >= 16 && temp <= 31) // C 
				nat = true;
		}
		//
		return nat;
	}

    int16_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
};
	

class P2PAddress 
{
public:
	bool IsNAT() const { return !(0xffffffff == subnetIP.sin_addr.s_addr);};
	NetAddress    outerIP;
	NetAddress    subnetIP;
	bool operator == (const P2PAddress& addr) const 
	{
		return ((addr.outerIP.sin_addr.s_addr == outerIP.sin_addr.s_addr) && (addr.subnetIP == subnetIP));
	};

	P2PAddress& operator = (const P2PAddress& addr)  
	{
		outerIP = addr.outerIP;
		subnetIP = addr.subnetIP;
		return *this;
	};
};


class CorePeerInfo {
public:
    CorePeerInfo() {memset(this, 0, sizeof(CorePeerInfo));};
    unsigned layer:8;       
    unsigned isMaxIn:1;     
    unsigned isMaxOut:1;    
    unsigned isMaxFreeIn:1; 
    unsigned isMaxFreeOut:1;
    unsigned isSuperPeer:1; 
    unsigned :19;           
};


class PeerInfoWithAddr : public CorePeerInfo, public P2PAddress {};

class TransferInfo {
public:
    TransferInfo() { memset(this, 0, sizeof(TransferInfo)); };
    uint64_t totalDownBytes; 
    uint64_t totalUpBytes;    
    float    currDownSpeed;   
    float    currUpSpeed;     
    float    avgDownSpeed;    
    float    avgUpSpeed;      
};

class BlockInterval {
public:
    BlockInterval() : start(UINT32_MAX) {};
    BlockInterval(unsigned int s, unsigned int ss) : start(s), size(ss) {};

    static bool cmp_size(const BlockInterval& i1, const BlockInterval& i2) {
        return (i1.size < i2.size);
    };
    static bool cmp_start(const BlockInterval& i1, const BlockInterval& i2) {
        return (i1.start < i2.start);
    };
    bool operator == (const BlockInterval& a) const {
        return (start == a.start && size == a.size);
    };
    BlockInterval& operator=(const BlockInterval& another) {
        start = another.start;
        size = another.size;
        return *this;
    }

    static void and_op(const BlockInterval& i1, const BlockInterval& i2, BlockInterval& result) {
        result.start = max<unsigned int>(i1.start, i2.start);	
        result.size = min<unsigned int>(i1.start+i1.size, i2.start+i2.size);
        if(result.size > result.start)
            result.size = result.size-result.start;
        else
            result.size = 0;
    };

public:
    uint32_t  start;
    uint32_t  size;
};

class MD5_Hash_Str
{
public:
	bool operator < (const MD5_Hash_Str other) const
	{
		return strncasecmp(hash_, other.hash_, MD5_LEN) < 0;
	}
	bool operator == (const MD5_Hash_Str other) const
	{
		return strncmp(hash_, other.hash_, MD5_LEN) == 0;
	}
	MD5_Hash_Str& operator = (const MD5_Hash_Str other)
	{
		memcpy(hash_, other.hash_, sizeof(hash_));
		blockInterval = other.blockInterval;
	}
	char hash_[MD5_LEN+1];
	BlockInterval blockInterval;
};

#define MEDIA_BLOCK_SIZE 16384

#define MAX_BUFFER_SIZE 1024

#define UUID_LENGTH 16

} // namespace rs::core

#endif
