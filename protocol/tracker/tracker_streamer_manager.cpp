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
#include "tracker_streamer_manager.h"
#include "core/logger.h"
#include "third_party/md5/md5.h"
#include "tracker_sp_connector.h"
#include <stdio.h>

namespace rs {
namespace protocol {
namespace tracker {

TrackerStreamManager *TrackerStreamManager::p = new TrackerStreamManager;
TrackerStreamManager *TrackerStreamManager::instance() { return p; }

TrackerStreamManager::TrackerStreamManager() { channel_map.clear(); }
TrackerStreamManager::~TrackerStreamManager() {
    for (const auto& val: channel_map) {
        ChannelNode *temp = val.second;
        delete temp;
    }
    channel_map.clear();
}

int TrackerStreamManager::initialize(const string &block_data_store_path) { return 0; }


int TrackerStreamManager::timer_check() {
    for(CMIt it = channel_map.begin(); it != channel_map.end(); ) {
        CMIt i = it;
        it++;
        time_t curr_time;
        time(&curr_time);
        // delete the channel if it idles too long
        if (curr_time - i->second->last_recv_report_time_ > 
            MaxIdleTime::MAX_IDLE_TIME_SEC) {
            RSLOGE("Channel removed due to long time idle.\n");

            // delete this channel
            ChannelNode* temp = i->second;
            delete temp;
            channel_map.erase(i);
        }
    }
   return 0;
}

int TrackerStreamManager::signal_get_res_interval(MD5_Hash_Str Channel_hash) {
    for (const auto& val: channel_map) {
        for (int i = 0; i < val.second->resourceCount; i++) {
            if (val.second->pHash[i] == Channel_hash) {
                RsSpTracker *sp =
                    static_cast<RsSpTracker *>(val.second->spService);
                if (sp) {
                    // sp->send_TS2SP_GET_RES_LIST(); //Kevin.Wen,fixme
                    return 0;
                }
            }
        }
    }

    return 1;
}

int TrackerStreamManager::get_channel_interval(MD5_Hash_Str Channel_hash,
                                    BlockInterval &blockInterval) {
    for (const auto& val: channel_map) {
        for (int i = 0; i < val.second->resourceCount; i++) {
            if (val.second->pHash[i] == Channel_hash) {
                blockInterval = val.second->pHash[i].blockInterval;
                return 0;
            }
        }
    }

    return 1;
}

int TrackerStreamManager::get_channel_count() { return channel_map.size(); }

int TrackerStreamManager::insert_channel(map_str strMd5, ChannelNode *chnl) {
    ChannelNode Node;
    if (get_channel(strMd5, &Node) == -1) {
        try {
            ChannelNode *pNode = new ChannelNode();
            if (!pNode) {
                RSLOGE("pNode = new ChannelNode() error | new_channel.\n");
                return 0;
            }

            *pNode = *chnl;
            bool ret =
                channel_map
                    .insert(std::pair<map_str, ChannelNode *>(strMd5, pNode))
                    .second;
            if (!ret) {
                RSLOGE("channel_map.insert error | new_channel.\n");
                delete pNode;
                pNode = NULL;
                return 0;
            }
        } catch (...) {
            RSLOGE("new ChannelNode Exception error  | new_channel.\n");
        }
    } else {
        ChannelNode *pNode = get_node(strMd5);
        if (NULL != pNode) {
            *pNode = *chnl;
        }
    }
    return 0;
}

int TrackerStreamManager::delete_channel(map_str uuid) {
    CMIt it = channel_map.find(uuid);
    if (it == channel_map.end()) {
        return NULL;
    }

    delete it->second;
    channel_map.erase(it);

    return 0;
}

int TrackerStreamManager::get_channel(map_str chnlhash, ChannelNode *Node) {
    CCMIt it = channel_map.find(chnlhash);
    if (it == channel_map.end()) {
        return -1;
    }

    *Node = *(it->second);
    return 0;
}

ChannelNode *TrackerStreamManager::get_node(map_str chnlhash) {
    CCMIt it = channel_map.find(chnlhash);
    if (it == channel_map.end()) {
        return NULL;
    }
    return it->second;
}

int TrackerStreamManager::get_cp_address(NetAddress *&pSPAddr, map_str chnlhash) {
    pSPAddr = new NetAddress[1];
    CCMIt it = channel_map.find(chnlhash);
    if (it == channel_map.end()) {
        delete pSPAddr;
        pSPAddr = NULL;
        return -1;
    }

    pSPAddr[0] = it->second->spAddress;
    return 0;
}

int TrackerStreamManager::get_all_cp_address(NetAddress *&pSPAddr, int &inCount,
                                  map_str uuid) {
    inCount = get_channel_count();

    if (0 == inCount)
        return 0;

    get_cp_address(pSPAddr, inCount, uuid);
}

int TrackerStreamManager::get_cp_address(NetAddress *&pSPAddr, int &inCount,
                              map_str uuid) {
    NetAddress *sel = NULL;
    get_cp_address(sel, uuid);

    inCount = get_channel_count() - 1;
    if (0 >= inCount) {
        pSPAddr = NULL;
        inCount = 0;
        delete sel;
        return 0;
    }

    pSPAddr = new NetAddress[inCount];

    int index = 0;
    for (const auto& val: channel_map) {
        if (NULL == sel || sel[0] != val.second->spAddress) {
            pSPAddr[index] = val.second->spAddress;
            index++;
        }

    }
    inCount = index;

    delete sel;
    return index;
}

int TrackerStreamManager::get_cp_address(NetAddress *&pSPAddr, int inCount) {
    time_t seed;
    time(&seed);
    srand(seed);

    pSPAddr = new NetAddress[inCount];

    int imax = channel_map.size();
    int imaxPos = imax - inCount;
    int iPos = 0;
    if (imaxPos > 0) {
        int iR = rand();
        iPos = iR % imaxPos;
    }

    int index = 0;
    int i = 0;
    for (const auto& val: channel_map) {
        if (i >= iPos) {
            if (index >= inCount) {
                break;
            }

            pSPAddr[index] = val.second->spAddress;
            index++;
        }
        i++;
    }

    return index;
}

int TrackerStreamManager::get_sp_address(MD5_Hash_Str resHash, NetAddress *&pSPAddr,
                              int inCount) {
    pSPAddr = new NetAddress[inCount];

    int index = 0;
    for (const auto& val: channel_map) {
        if (index >= inCount) {
            break;
        }
        for (int i = 0; i < val.second->resourceCount; i++) {
            if (val.second->pHash[i] == resHash) {
                pSPAddr[index] = val.second->spAddress;
                index++;
                break;
            }
        }
    }

    return index;
}

} // namespace tracker
} // namespace protocol
} // namespace rs
