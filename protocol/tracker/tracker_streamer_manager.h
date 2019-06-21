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
#ifndef PROTOCOL_TRACKER_STREAMER_MANAGER_H_
#define PROTOCOL_TRACKER_STREAMER_MANAGER_H_

#include <map>
#include <string.h>
#include <string>

#include "core/struct_define.h"

using namespace std;
using namespace core;

namespace protocol
{
namespace tracker
{

class StreamMgr
{
private:
    StreamMgr();
    static StreamMgr *p;

public:
    virtual ~StreamMgr();

public:
    //for single instance
    static StreamMgr *instance();

    int initialize(const string &block_data_store_path);

    //return is the count result
    int get_channel_count();
    // add a new channel
    int insert_channel(map_str strMd5, ChannelNode *chnl);
    int delete_channel(map_str uuid);

    // find a channel
    int get_channel(map_str chnlhash, ChannelNode *Node);

    // find sp address
    int get_cp_address(NetAddress *&pSPAddr, map_str chnlhash);
    int get_cp_address(NetAddress *&pSPAddr, int &inCount, map_str uuid);
    int get_all_cp_address(NetAddress *&pSPAddr, int &inCount, map_str uuid);
    int get_cp_address(NetAddress *&pSPAddr, int inCount);
    int get_sp_address(MD5_Hash_Str resHash, NetAddress *&pSPAddr, int inCount);

    //
    int get_channel_interval(MD5_Hash_Str Channel_hash, BlockInterval &blockInterval);

    int signal_get_res_interval(MD5_Hash_Str Channel_hash);
    //
protected:
    ChannelNode *get_node(map_str chnlhash);

private:
    typedef std::map<map_str, ChannelNode *> ChannelHashMap;
    typedef std::map<map_str, ChannelNode *>::iterator CMIt;
    typedef std::map<map_str, ChannelNode *>::const_iterator CCMIt;
    typedef std::pair<map_str, ChannelNode *> CMPair;

    ChannelHashMap chnl_map_;

    enum
    {
        // if a live channel stopped receiving block from cs for more than MAX_IDLE_TIME_SEC, it will be deleted
        MAX_IDLE_TIME_SEC = 100,
    };
};

} // namespace tracker
} // namespace protocol

#endif
