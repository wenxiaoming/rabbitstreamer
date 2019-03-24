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

#ifndef PROTOCOL_BUFFER_QUEUE_H_
#define PROTOCOL_BUFFER_QUEUE_H_

#include <vector>
#include <string>
#include <time.h>
#include "core/core_struct.h"

using namespace std;

using namespace core;

namespace protocol {

enum BUFFER_FLAG
{
    BUFFER_WRITING,
    BUFFER_READING,
    BUFFER_AVAILABLE
};

typedef struct media_buffer
{
    BUFFER_FLAG flag;
    int block_id;
    int block_size;
    char* buffer;
}media_buffer;

typedef struct media_type_header
{
    char* media_type;
    int header_size;
}media_type_header;

//RsBufferQueue is similar to android's gui's BufferQueue
//it will use queue_buffer to get one free buffer for storing buffer from capture server
//and the socket from NP will use dequeue_buffer to get one media buffer for consuming in the player.
class RsBufferQueue {
public:
    RsBufferQueue(const MD5_Hash_Str& hash, const string& name, bool source);
    virtual ~RsBufferQueue();
public:
    void reserve_buffer(int size);
    media_buffer* queue_buffer();
    media_buffer* dequeue_buffer(int block_id);
    int update_buffer_attr(int block_id, BUFFER_FLAG flag);
public:
    int get_buffer_interval(int& start, int& end);
    MD5_Hash_Str get_source_hash(){return source_hash;}
    string get_source_name(){return source_name;}
    time_t get_create_time(){return create_time;}
    bool is_source_flag(){return is_source;}
public:
    void set_header(const char* header, int size);
    void get_header(char*& header, int& size);
private:
    //disable copy and assign
    RsBufferQueue (const RsBufferQueue &);
    RsBufferQueue &operator= (const RsBufferQueue &);
private:
    media_type_header media_header;
    // the time when source is created
    time_t create_time;
    // hash code of this source
    MD5_Hash_Str source_hash;
    // name of this source
    string source_name;
    // it is source or cached
    bool is_source;
private:
    int queue_index;
    int dequeue_index;
    int buffer_number;
    int min_block_id; //the min block id
    int min_block_index;//the min block's index in the queue
    bool buffer_full_flag;
    vector<media_buffer*> buffer_vector;
};

} /* namespace protocol */

#endif /* PROTOCOL_BUFFER_QUEUE_H_ */
