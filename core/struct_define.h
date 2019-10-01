/*
 *  Openmysee
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */				 

#ifndef CORE_STRUCT_DEFINE_H__
#define CORE_STRUCT_DEFINE_H__

#include <time.h>
#include "../core/core_struct.h"
#include "../core/interval_array.h"
//#include "../protocol/tracker/tracker_sp_protocol.h"

//using namespace protocol;

//using namespace tracker;
namespace protocol {
	namespace tracker {
		class RsSpTracker;
	}
}

//class RsSpTracker;
namespace rs {
namespace core {

class map_str {
public:
	bool operator < (const map_str other) const {
		return strncasecmp(str_, other.str_, UUID_LENGTH) < 0;
	}

	bool operator != (const map_str other) const {
		return strncasecmp(str_, other.str_, UUID_LENGTH) != 0;
	}

	map_str& operator = (const map_str other) {
		memcpy(str_, other.str_, sizeof(str_));

		return *this;
	}

	char str_[UUID_LENGTH+1];
};
//channelNode
struct  ChannelNode {
	ChannelNode() {
		userID = 0;                  
		pswd[0] = 0;                   
		servicePort = 0;               
		resourceCount = 0;             
		conNum = 0;                     
		exceedMaxConn = 0;              
		bandwidth = 0;                  
		memset(&spAddress, 0, sizeof(NetAddress));                  
		
        time(&last_recv_report_time_);
		pHash = NULL;                      
		spService = NULL;

	}

	~ChannelNode() {
		delete_pHash();
	}

	ChannelNode& operator = (const ChannelNode& pNode) {
		userID = pNode.userID;                    
		memcpy(pswd, pNode.pswd, sizeof(pswd));
		servicePort = pNode.servicePort;                
		resourceCount = pNode.resourceCount;              
		conNum = pNode.conNum;                     
		exceedMaxConn = pNode.exceedMaxConn;             
		bandwidth = pNode.bandwidth;                  
		memcpy(&spAddress, &(pNode.spAddress), sizeof(NetAddress));
		spService = pNode.spService;

        time(&last_recv_report_time_);

		new_pHash();

		for (int i =0; i < resourceCount; i++)
		{
			pHash[i] = pNode.pHash[i];
		}

		return (*this);
	}
	
	time_t last_recv_report_time_;
	uint32_t userID;
	uint8_t pswd[33];
	uint16_t servicePort;
	uint32_t resourceCount;
	uint16_t conNum;
	bool exceedMaxConn;
	float bandwidth;
	NetAddress spAddress;
	//RsSpTracker* spService;
	void* spService;
	
	MD5_Hash_Str* pHash;

	void new_pHash() {
		if (0 < resourceCount) {
			pHash = new MD5_Hash_Str[resourceCount];
		}
		else {
			pHash = NULL;
		}
	}

	void delete_pHash() {
		if (NULL != pHash)
			delete []pHash;
		//
		pHash = NULL;
	}
};

//npnode
struct  NPNode {
	 map_str     digits;                 
	 time_t      last_recv_report_time_;
	 float       fVersion;                   //version of client(float)
	 uint16_t  port;                       //listening port(USHORT)
	 P2PAddress  clientAddress;              ////

	 CorePeerInfo coreInfo;
	 TransferInfo transInfo;
	 uint32_t   playingBlock;              //playing block(UINT32)
	 uint16_t   currentBufferingTime;      //Current Buffering Time(UINT16)
	 uint16_t   bufferedCount;             //Buffered Count(UINT16)
	 uint16_t   bufferedTime;              //Buffered Time(UINT16)
	 uint16_t   connectFailCount;          //Connect Fail Count(UINT16)
	 uint16_t   incomingConnectionCount;   //Incoming Connection Count(UINT16)
	 uint16_t   outgoingConnectionCount;   //Outgoing Connection Count(UINT16)
	 uint16_t   AvgincomingConnectionElapsedTime;//Avg Incoming Connection Elapsed Time(UINT16)
	 uint16_t   AvgoutgoingConnectionElapsedTime;//Avg OutgoingConnection Elapsed Time(UINT16)
	 float        msgPercent;                        //Message Percent(float)

	 uint8_t    countInterval;
	 IntervalArray intervalArray;

	 MD5_Hash_Str channelID_md5;             

	 NPNode() {
		 memset(this, 0, sizeof(NPNode));
		 time(&last_recv_report_time_);
	 }

	 NPNode& operator = (const NPNode& node) {
		 digits = node.digits;                  
		 last_recv_report_time_ = node.last_recv_report_time_;
		 fVersion = node.fVersion;                   //version of client(float)
		 port = node.port;                     //listening port(USHORT)
		 clientAddress = node.clientAddress;              ////

		 coreInfo = node.coreInfo;
		 transInfo = node.transInfo;
		 playingBlock = node.playingBlock;            //playing block(UINT32)
		 currentBufferingTime = node.currentBufferingTime;     //Current Buffering Time(UINT16)
		 bufferedCount = node.bufferedCount;             //Buffered Count(UINT16)
		 bufferedTime = node.bufferedTime;              //Buffered Time(UINT16)
		 connectFailCount = node.connectFailCount;          //Connect Fail Count(UINT16)
		 incomingConnectionCount = node.AvgincomingConnectionElapsedTime; //Incoming Connection Count(UINT16)
		 outgoingConnectionCount = node.outgoingConnectionCount;//Outgoing Connection Count(UINT16)
		 AvgincomingConnectionElapsedTime = node.AvgincomingConnectionElapsedTime;//Avg Incoming Connection Elapsed Time(UINT16)
		 AvgoutgoingConnectionElapsedTime = node.AvgoutgoingConnectionElapsedTime;//Avg OutgoingConnection Elapsed Time(UINT16)
		 msgPercent = node.msgPercent;                      //Message Percent(float)

		 countInterval = node.countInterval;
		 intervalArray = node.intervalArray;

		 channelID_md5 = node.channelID_md5;

		 return *this;
	 }
	 
};

} // namespace rs::core

#endif
