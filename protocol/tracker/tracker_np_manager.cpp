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

TrackerNpCoordinator *TrackerNpCoordinator::p = new TrackerNpCoordinator();
TrackerNpCoordinator *TrackerNpCoordinator::instance() { return p; }

TrackerNpCoordinator::TrackerNpCoordinator() { npnode_map.clear(); }

TrackerNpCoordinator::~TrackerNpCoordinator() {
    for (CMIt it = npnode_map.begin(); it != npnode_map.end(); ++it) {
        NPNode *temp = it->second;
        delete temp;
    }
    //
    npnode_map.clear();
}

int TrackerNpCoordinator::timer_check() {
    for (CMIt it = npnode_map.begin(); it != npnode_map.end();) {
        CMIt i = it;
        it++;
        time_t curr_time;
        // delete the NPNode if it idles too long
        time(&curr_time);
        if ((curr_time - (i->second->last_recv_report_time_)) >
            MAX_IDLE_TIME_SEC) {
            RSLOGE("NPNode removed due to long time idle.\n");

            // delete this NPNode
            NPNode *temp = i->second;
            delete temp;

            npnode_map.erase(i);
        }
    }

    return 0;
}

int TrackerNpCoordinator::insert_Node(map_str digits, NPNode *node) {
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

int TrackerNpCoordinator::deleteNode(map_str digits) {
    CMIt it = npnode_map.find(digits);
    //
    if (it != npnode_map.end()) {
        delete (it->second);

        npnode_map.erase(it);
    }
    //
    return 0;
}

int TrackerNpCoordinator::get_Node(map_str digits, NPNode *node) {
    CCMIt it = npnode_map.find(digits);
    if (it == npnode_map.end()) {
        return -1;
    }
    //
    *node = *(it->second);
    return 0;
}

NPNode *TrackerNpCoordinator::get_Node(map_str digits) {
    CCMIt it = npnode_map.find(digits);
    if (it == npnode_map.end()) {
        return NULL;
    }
    //
    return it->second;
}

int TrackerNpCoordinator::get_np_address(MD5_Hash_Str resHash, map_str uuid,
                                         PeerInfoWithAddr *&pPeerInfoWithAddr,
                                         int inCount, uint32_t currentblockID) {
    /*
        随机取node的算法思想

        因为用的是map存储的node节点，现在要找的是：在同看一个频道、不是自己、还拥有需要的数据块。
        并且要随机的在整个map表中找出20个需要的node。

        这里用的方法是：
        1、取得map节点的个数
        2、把map按30一组分成 iRange = mapsize/ 30 组
        3、把从0.....iRange个数依序存在向量数组中theVector
        4、随机的从theVector中取出一个数，作为查找map的开始点
        5、一组查完了再从4开始重复，知道找到自己需要的节点数
    */

    pPeerInfoWithAddr = new PeerInfoWithAddr[inCount];

    typedef vector<int> INTVECTOR;
    INTVECTOR theVector;

    int iSize = npnode_map.size();
    int iRange = iSize / 30 + 1;

    for (int i = 0; i < iRange; i++) {
        theVector.push_back(i);
    }

    int r1 = rand();

    int index = 0;
    do {
        int iPos = 0;
        //
        for (int f = 0; f < iRange; f++) {
            int r1 = rand();
            if (r1 != 0) {
                int iVector = r1 % iRange;
                iPos = theVector[iVector];
                theVector[iVector] = -1;
            }
            //
            if (-1 == iPos)
                continue;
        }

        if (-1 == iPos)
            break;

        CCMIt it = npnode_map.begin();
        for (int m = 0; m < iPos * 30; m++) {
            it++;
        }

        for (int iLimit = 0; it != npnode_map.end(); it++) {
            if (30 <= iLimit) {
                break;
            }
            //
            if (index >= inCount) {
                break;
            }
            //
            if (it->second->channelID_md5 == resHash //同一个频道
                && it->second->digits != uuid        //不是自己
                && it->second->intervalArray.FindBlock(
                       currentblockID)) { //拥有自己需要的数据块
                *(CorePeerInfo *)&pPeerInfoWithAddr[index] =
                    it->second->coreInfo;
                *(P2PAddress *)&pPeerInfoWithAddr[index] =
                    it->second->clientAddress;
                index++;
            }
        }

        if (it != npnode_map.end())
            break;
        //
        if (index >= inCount) {
            break;
        }

    } while (true);

    return index;
}

} // namespace tracker
} // namespace protocol
} // namespace rs