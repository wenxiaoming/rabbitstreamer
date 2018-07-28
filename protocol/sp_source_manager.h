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
#ifndef PROTOCOL_SP_SOURCE_MANAGER_H_
#define PROTOCOL_SP_SOURCE_MANAGER_H_

#include <map>
#include <list>
#include "core/core_struct.h"
#include "protocol/buffer_queue.h"

using namespace std;

struct source_status
{
    // hash of channel
    MD5_Hash_Str chnl_hash_;
    string program_name;
    uint32_t program_time;
    // Channel Name
    string chnl_name_;
    // Channel created time
    time_t created_time_;
    // block interval of channel
    BlockInterval block_inter_;
};

//manage the source from capture server, it is single instance
class RsSourceManager {
private:
    RsSourceManager(){}
    static RsSourceManager* p;
public:
    virtual ~RsSourceManager();
public:
    //for single instance
    static RsSourceManager* instance();
public:
    void reserve_buffer(MD5_Hash_Str hash,int size);
    void queue_buffer(MD5_Hash_Str hash);
    void dequeue_buffer(MD5_Hash_Str hash);
public:
    // add a new channel
    int create_source (const MD5_Hash_Str& source_hash, const string& source_name, bool source);
    // save a block into queue
    int queue_block (const MD5_Hash_Str& chnl_hash, char** block, int size);
    // get a block from queue
    int dequeue_block (const MD5_Hash_Str& chnl_hash, int block_id, char*& block,
            uint32_t& block_size, bool& notfound);

    // update buffer's flag
    int set_block_available (const MD5_Hash_Str& chnl_hash, int block_id);
    // get source channel list
    void get_source_list(list<source_status>& source_list);
    // save media header
    int add_header (const MD5_Hash_Str& chnl_hash, const char* header, int size);
    // get media header
    int get_header (const MD5_Hash_Str& chnl_hash, char*& header, int& size);

    int write_source_channel_list_txt();
    //not implemented
    ////////////////////////////////////////////
    int initialize (const string& data_store_path, const string& xml_store_path, const string& tracker_addr);

    // check the status of channel
    void check_channel();

	bool find_channel_is_source(const MD5_Hash_Str& chnl_hash);

    // set this channel as deactivated
    int deactivate_channel(const MD5_Hash_Str& chnl_hash);

    // check if this channel exists
    bool find_channel(const MD5_Hash_Str& chnl_hash) {return get_buffer_queue(chnl_hash) != NULL;};

    // lock following operations of Channels
    // save a block into file
    //int add_block (const MD5_Hash_Str& chnl_hash, char* block, int size);

    int get_max_blockid(const MD5_Hash_Str& chnl_hash, int& block_id);

private:
    RsBufferQueue* get_buffer_queue(MD5_Hash_Str hash);
private:
    map<MD5_Hash_Str, RsBufferQueue*> source_hashmap;
    string tracker_address;
};

#endif /* PROTOCOL_SP_SOURCE_MANAGER_H_ */
