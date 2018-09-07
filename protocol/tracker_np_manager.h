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
#ifndef PROTOCOL_TRACKER_NP_MANAGER_
#define PROTOCOL_TRACKER_NP_MANAGER_

#include <map>
#include "../core/struct_define.h"

// manager of all channels
class TrackerNpCoordinator
{
private:
    TrackerNpCoordinator();
    static TrackerNpCoordinator* p;
public:
    virtual ~TrackerNpCoordinator();
public:
    //for single instance
    static TrackerNpCoordinator* instance();
	
	int timer_check();
    // add a new channel
    int insert_Node(map_str digits, NPNode* node);
    // delete this node
    int deleteNode(map_str digits);

    // find a channel
	int get_Node(map_str digits, NPNode* node);
   
	// find sp address
	int get_np_address(MD5_Hash_Str resHash, map_str uuid,
		              PeerInfoWithAddr*& pPeerInfoWithAddr, 
					  int inCount, uint32_t currentblockID);

protected:
	NPNode* get_Node(map_str digits );

private:
	typedef std::map<map_str, NPNode*> HashMap;
    typedef std::map<map_str, NPNode*>::iterator CMIt;
    typedef std::map<map_str, NPNode*>::const_iterator CCMIt;
    typedef std::pair<map_str, NPNode*> CMPair;

    HashMap NPNode_map_;
    
    enum
    {
        // if a live channel stopped receiving block from cs for more than MAX_IDLE_TIME_SEC, it will be deleted
        MAX_IDLE_TIME_SEC = 600, 
    };
};

#endif

