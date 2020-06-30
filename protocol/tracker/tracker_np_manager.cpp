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
#include "tracker_np_manager.h"
#include "core/logger.h"
#include "md5.h"
#include <vector>

using namespace std;

namespace rs {
namespace protocol {
namespace tracker {

RsTrackerNpCoordinator *RsTrackerNpCoordinator::p =
    new RsTrackerNpCoordinator();
RsTrackerNpCoordinator *RsTrackerNpCoordinator::instance() { return p; }

RsTrackerNpCoordinator::RsTrackerNpCoordinator() { npnode_map.clear(); }

RsTrackerNpCoordinator::~RsTrackerNpCoordinator() {
    for (const auto &val : npnode_map) {
        NPNode *temp = val.second;
        delete temp;
    }
    //
    npnode_map.clear();
}

int RsTrackerNpCoordinator::timer_check() {
    for (CMIt it = npnode_map.begin(); it != npnode_map.end();) {
        CMIt i = it;
        it++;
        time_t curr_time;
        // delete the NPNode if it idles too long
        time(&curr_time);
        if ((curr_time - (i->second->last_recv_report_time_)) >
            MaxIdleTime::MAX_IDLE_TIME_SEC) {
            RSLOGE("NPNode removed due to long time idle.\n");

            // delete this NPNode
            NPNode *temp = i->second;
            delete temp;

            npnode_map.erase(i);
        }
    }

    return 0;
}

int RsTrackerNpCoordinator::insert_Node(map_str digits, NPNode *node) {
    NPNode *pNode = get_Node(digits);
    if (NULL == pNode) {
        // create a new channel
        RSLOGE("creating new NPNode(%s)\n", digits.str_);

        try {
            pNode = new NPNode();
            if (!pNode) {
                RSLOGE("node = new NPNode(); error | new_Node.\n");
                return 0;
            }

            *pNode = *node;

            bool ret =
                npnode_map.insert(std::pair<map_str, NPNode *>(digits, pNode))
                    .second;
            if (!ret) {
                RSLOGE("npnode_map.insert error | new_Node.\n");
                delete pNode;
                pNode = NULL;
                return 0;
            }
        } catch (...) {
            RSLOGE("new NPNode Exception error | new_Node.\n");
            pNode = NULL;
        }
    } else {
        *pNode = *node;
    }
    //
    return 0;
}

int RsTrackerNpCoordinator::deleteNode(map_str digits) {
    CMIt it = npnode_map.find(digits);
    //
    if (it != npnode_map.end()) {
        delete (it->second);

        npnode_map.erase(it);
    }
    //
    return 0;
}

int RsTrackerNpCoordinator::get_Node(map_str digits, NPNode *node) {
    CCMIt it = npnode_map.find(digits);
    if (it == npnode_map.end()) {
        return -1;
    }
    //
    *node = *(it->second);
    return 0;
}

NPNode *RsTrackerNpCoordinator::get_Node(map_str digits) {
    CCMIt it = npnode_map.find(digits);
    if (it == npnode_map.end()) {
        return NULL;
    }
    //
    return it->second;
}

// get the node peers's address, then we can connect with these peers to get media block
int RsTrackerNpCoordinator::get_np_address(MD5_Hash_Str resHash, map_str uuid,
                                           PeerInfoWithAddr *&pPeerInfoWithAddr,
                                           int inCount,
                                           uint32_t currentblockID) {
    
    // todo, no need to allocate memory every time
    pPeerInfoWithAddr = new PeerInfoWithAddr[inCount];

    vector<int> groupID;

    // split the npnode_map into groups, one group has 30 nodes
    int iSize = npnode_map.size();
    int iRange = iSize / 30 + 1;

    // store the group id in groupID
    for (int i = 0; i < iRange; i++) {
        groupID.push_back(i);
    }

    int r1 = rand();

    int index = 0;
    do {
        int iPos = 0;

        // find the group id
        for (int f = 0; f < iRange; f++) {
            int r1 = rand();
            if (r1 != 0) {
                int id = r1 % iRange;
                iPos = groupID[id];
                groupID[id] = -1;
            }

            // we have tried this group before
            if (-1 == iPos)
                continue;
        }

        // we have tried all groups, just exits the loop
        if (-1 == iPos)
            break;

        // seek to the specific group by its group id iPos
        CCMIt it = npnode_map.begin();
        for (int m = 0; m < iPos * 30; m++) {
            it++;
        }

        for (int iLimit = 0; it != npnode_map.end(); it++) {
            if (30 <= iLimit) { // try 30 times at most
                break;
            }

            // we have got enough nodes
            if (index >= inCount) {
                break;
            }

            if (it->second->channelID_md5 == resHash // same channel
                && it->second->digits != uuid        // not himself
                && it->second->intervalArray.FindBlock(
                       currentblockID)) { // the node has the block whose id is currentblockID
                *(CorePeerInfo *)&pPeerInfoWithAddr[index] =
                    it->second->coreInfo;
                *(P2PAddress *)&pPeerInfoWithAddr[index] =
                    it->second->clientAddress;
                index++;
            }
        }

        if (it != npnode_map.end())
            break;
        if (index >= inCount) {
            break;
        }

    } while (true);

    return index;
}

} // namespace tracker
} // namespace protocol
} // namespace rs