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

#include "sp_source_manager.h"
#include "core/error_code.h"
#include <stdio.h>

namespace rs {
namespace protocol {
namespace sp {

RsSourceManager *RsSourceManager::p = new RsSourceManager;
RsSourceManager *RsSourceManager::instance() { return p; }

RsSourceManager::~RsSourceManager() {}

RsBufferQueue *RsSourceManager::get_buffer_queue(MD5_Hash_Str hash) {
    map<MD5_Hash_Str, RsBufferQueue *>::iterator iter =
        source_hashmap.find(hash);
    if (iter == source_hashmap.end())
        return NULL;

    return iter->second;
}

int RsSourceManager::write_source_channel_list_txt() {
    char temp[512];
    string xml_store_path_ = "./";
    sprintf(temp, ("%schannel.txt"), xml_store_path_.c_str());
    FILE *txt_file = fopen(temp, "wt");
    if (txt_file == NULL) {
        return -1;
    }

    map<MD5_Hash_Str, RsBufferQueue *>::iterator iter = source_hashmap.begin();
    for (; iter != source_hashmap.end(); ++iter) {
        // add source channel to list
        if (iter->second->is_source_flag()) {
            sprintf(
                temp,
                ("Name: %s\r\nURL: myseelitetest://%s/%s/%s\r\nStart: %d\r\n"),
                iter->second->get_source_name().c_str(),
                tracker_address.c_str(), iter->second->get_source_hash().hash_,
                iter->second->get_source_name().c_str(),
                iter->second->get_create_time());
            if (fwrite(temp, 1, strlen(temp), txt_file) != strlen(temp)) {
                fclose(txt_file);
                return -1;
            }
        }
    }

    fclose(txt_file);
    return 0;
}

int RsSourceManager::initialize(const string &data_store_path,
                                const string &xml_store_path,
                                const string &tracker_addr) {
    int ret = ERROR_SUCCESS;
    tracker_address = tracker_addr;
    return ret;
}

int RsSourceManager::create_source(const MD5_Hash_Str &source_hash,
                                   const string &source_name, bool source) {
    int ret = ERROR_SUCCESS;

    RsBufferQueue *buffer_queue = get_buffer_queue(source_hash);
    if (buffer_queue) {
    } else {
        buffer_queue = new RsBufferQueue(source_hash, source_name, source);
        if (buffer_queue) {
            buffer_queue->reserve_buffer(MAX_BUFFER_SIZE);
        }

        bool ret = source_hashmap
                       .insert(std::pair<MD5_Hash_Str, RsBufferQueue *>(
                           source_hash, buffer_queue))
                       .second;
        if (!ret) {
            delete buffer_queue;
            return ERROR_SOURCE_MGR_FAILT_TO_CREATE_SOURCE;
        }
    }
    return ret;
}

int RsSourceManager::queue_block(const MD5_Hash_Str &chnl_hash, char **block,
                                 int size) {
    int ret = ERROR_SUCCESS;

    RsBufferQueue *buffer_queue = get_buffer_queue(chnl_hash);
    if (!buffer_queue) {
        return ERROR_SOURCE_MGR_NOT_FOUND;
    }

    media_buffer *buf = buffer_queue->queue_buffer();
    *block = buf->buffer;
    return ret;
}

int RsSourceManager::dequeue_block(const MD5_Hash_Str &chnl_hash, int block_id,
                                   char *&block, uint32_t &block_size,
                                   bool &notfound) {
    int ret = ERROR_SUCCESS;

    RsBufferQueue *buffer_queue = get_buffer_queue(chnl_hash);
    if (!buffer_queue) {
        return ERROR_SOURCE_MGR_NOT_FOUND;
    }

    media_buffer *buf = buffer_queue->dequeue_buffer(block_id);
    if (buf) {
        block = buf->buffer;
        block_size = buf->block_size;
        notfound = false;
    } else
        notfound = true;

    return ret;
}

int RsSourceManager::set_block_available(const MD5_Hash_Str &chnl_hash,
                                         int block_id) {
    int ret = ERROR_SUCCESS;

    RsBufferQueue *buffer_queue = get_buffer_queue(chnl_hash);
    if (!buffer_queue) {
        return ERROR_SOURCE_MGR_NOT_FOUND;
    }
    buffer_queue->update_buffer_attr(block_id, BUFFER_AVAILABLE);
    return ret;
}

void RsSourceManager::get_source_list(list<source_status> &source_list) {
    map<MD5_Hash_Str, RsBufferQueue *>::iterator iter = source_hashmap.begin();
    source_status state;
    for (; iter != source_hashmap.end(); iter++) {
        int start = 0;
        int end = 0;
        state.chnl_hash = iter->second->get_source_hash();
        state.chnl_name_ = iter->second->get_source_name();
        state.created_time_ = iter->second->get_create_time();
        iter->second->get_buffer_interval(start, end);
        // printf("start:%d end:%d\n", start, end);
        state.block_inter_.start = start;
        state.block_inter_.size = (end - start);
        source_list.insert(source_list.end(), state);
    }
}

// save media header
int RsSourceManager::add_header(const MD5_Hash_Str &chnl_hash,
                                const char *header, int size) {
    int ret = ERROR_SUCCESS;
    RsBufferQueue *buffer_queue = get_buffer_queue(chnl_hash);
    if (!buffer_queue) {
        return ERROR_SOURCE_MGR_NOT_FOUND;
    }
    buffer_queue->set_header(header, size);
    return ret;
}

// get media header
int RsSourceManager::get_header(const MD5_Hash_Str &chnl_hash, char *&header,
                                int &size) {
    int ret = ERROR_SUCCESS;
    RsBufferQueue *buffer_queue = get_buffer_queue(chnl_hash);
    if (!buffer_queue) {
        return ERROR_SOURCE_MGR_NOT_FOUND;
    }
    buffer_queue->get_header(header, size);
    return ret;
}

} // namespace sp
} // namespace protocol
} // namespace rs
