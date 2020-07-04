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
#include "sp_np_connector.h"
#include "core/error_code.h"
#include "core/message.h"
#include "core/streamer.h"
#include "sp_source_manager.h"
#include <st.h>
#include <stdio.h>

namespace rs {
namespace protocol {
namespace sp {

//#define DUMP_SEND_MEDIA

#ifdef DUMP_SEND_MEDIA
FILE *np_dump_file = NULL;
#endif

RsNpSpProtocol::RsNpSpProtocol(st_netfd_t stfd)
    : RsThread("npspprotocol"), st_fd(stfd) {
    io_socket = new RsSocket(st_fd);
    np_buffer = new RsBuffer;
    media_type_flag = false;
    cycle_interval_us = 1 * 1000; // sleep 1ms
    printf("%s\n this:0x%0x\n", __FUNCTION__, this);
#ifdef DUMP_SEND_MEDIA
    np_dump_file = fopen("dump_send", "wb");
    if (np_dump_file == NULL)
        printf("fail to open file\n");
#endif
    calculator = new RsBitrateCalculator("np");
}

RsNpSpProtocol::~RsNpSpProtocol() {
#ifdef DUMP_SEND_MEDIA
    if (np_dump_file)
        fclose(np_dump_file);
#endif
    if (io_socket)
        delete io_socket;

    if (np_buffer)
        delete np_buffer;

    if (calculator)
        delete calculator;
}

int RsNpSpProtocol::on_thread_start() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpSpProtocol::on_before_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpSpProtocol::send_media_type(char *resource_hash) {
    int ret = ERROR_SUCCESS;
    char *media_type = NULL;
    int size = 0;
    printf("%s\n", __FUNCTION__);

    MD5_Hash_Str hash;
    memcpy(hash.hash_, resource_hash, MD5_LEN);
    hash.hash_[MD5_LEN] = 0;
    ret = RsSourceManager::instance()->get_header(hash, media_type, size);
    if (ret != ERROR_SUCCESS)
        return ret;

    Sp2SpMediatype mediatype_msg;
    memcpy(mediatype_msg.resource_md5, resource_hash, MD5_LEN);
    mediatype_msg.media_type_size = size;
    mediatype_msg.media_type = &media_type[0];
#ifdef DUMP_SEND_MEDIA
    if (np_dump_file) {
        fwrite(mediatype_msg.media_type, 1, size, np_dump_file);
    }
#endif
    list<source_status> source_list;
    RsSourceManager::instance()->get_source_list(source_list);
    list<source_status>::iterator iter = source_list.begin();
    for (; iter != source_list.end(); iter++) {
        if (!memcmp(iter->chnl_hash.hash_, mediatype_msg.resource_md5,
                    MD5_LEN)) {
            mediatype_msg.block_start_id = 0; // iter->block_inter_.start;
            mediatype_msg.block_num = -1;     // iter->block_inter_.size;
            mediatype_msg.program_name = (char *)iter->chnl_name_.c_str();
            mediatype_msg.program_name_size = 2; // iter->chnl_name_.size();
            mediatype_msg.program_time = 0;      // iter->created_time_;
            mediatype_msg.channel_name = (char *)iter->chnl_name_.c_str();
            mediatype_msg.channel_name_size = 2; // iter->chnl_name_.size();
            break;
        }
    }

    char *payload = NULL;
    int payload_nb = 0;
    mediatype_msg.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        ret = io_socket->write(payload, payload_nb, &nsize);

    if ((ret != ERROR_SUCCESS) || (nsize != payload_nb))
        printf("%s ret:%d nsize:%d payload_nb:%d\n", __FUNCTION__, ret, nsize,
               payload_nb);

    media_type_flag = true;
    return ret;
}

int RsNpSpProtocol::send_one_block(char *resource_hash, uint8_t count,
                                   uint32_t id) {
    int ret = ERROR_SUCCESS;
    char *block = NULL;
    uint32_t size = 0;
    MD5_Hash_Str hash;
    memcpy(hash.hash_, resource_hash, MD5_LEN);
    hash.hash_[MD5_LEN] = 0;

    bool not_found = true;
    uint32_t block_id = id;
    ret = RsSourceManager::instance()->dequeue_block(hash, block_id, block,
                                                     size, not_found);
    if ((ret != ERROR_SUCCESS) || not_found)
        return ret;

    Sp2SpResponse response_msg;
    memcpy(response_msg.resource_md5, resource_hash, MD5_LEN);
    response_msg.block_data = block;
    response_msg.block_size = size;
    response_msg.block_id = block_id;

#ifdef DUMP_SEND_MEDIA
    if (np_dump_file) {
        fwrite(block, 1, size, np_dump_file);
    }
#endif

    char *payload = NULL;
    int payload_nb = 0;
    response_msg.pack(payload, payload_nb);
    ssize_t nsize;
    if ((payload != NULL) && (payload_nb != 0))
        io_socket->write(payload, payload_nb, &nsize);

    if ((ret != ERROR_SUCCESS) || (nsize != payload_nb))
        printf("%s ret:%d nsize:%d payload_nb:%d\n", __FUNCTION__, ret, nsize,
               payload_nb);

    calculator->update_buffersize(size);
    // printf("%s id:%d \n", __FUNCTION__, id);
    return ret;
}

int RsNpSpProtocol::send_push_list(char *resource_hash, uint8_t count,
                                   uint32_t *array) {
    int ret = ERROR_SUCCESS;
    int i = 0;
    if (count > 1)
        sort(array, array + count);
    for (; i < count; i++) {
        send_one_block(resource_hash, 1, array[i]);
        // st_usleep(5*1000);
    }

    return ret;
}

int RsNpSpProtocol::get_push_list(char *msg, int size) {
    printf("%s\n", __FUNCTION__);

    int ret = ERROR_SUCCESS;
    Sp2SpPushList pushlist_msg;
    RsStreamer streamer;
    streamer.initialize(msg, size);
    pushlist_msg.parse(&streamer);
    if (!media_type_flag)
        send_media_type(&(pushlist_msg.resource_md5[0]));
    // st_usleep(10000);
    if (pushlist_msg.block_count)
        send_push_list(&(pushlist_msg.resource_md5[0]),
                       pushlist_msg.block_count, pushlist_msg.block_array);
    return ret;
}

static int no_receive_counter = 0;

int RsNpSpProtocol::loop() {
    int ret = ERROR_SUCCESS;
    // get message size
    ssize_t ret_size;
    // read 4 bytes to get the message type
    if ((ret = np_buffer->fill_buffer(io_socket, 4)) != ERROR_SUCCESS) {
        if (ret != ERROR_SUCCESS) {
            // printf("np_buffer->fill_buffer(io_socket, 4) failed.
            // required_size=%d, ret=%d \n", 1, ret);
        }
        no_receive_counter++;
        if (no_receive_counter == 150) {
            loop_flag = false;
            close(st_netfd_fileno(st_fd));
            no_receive_counter = 0;
            printf("%s close this socket since idle too long\n", __FUNCTION__);
        }
        // printf("%s 1\n", __FUNCTION__);
        return ret;
    }
    // printf("%s 2\n", __FUNCTION__);
    char *temp = np_buffer->read_nbytes(4);
    int msg_size = 0;
    memcpy((char *)&msg_size, temp, 4);

    // read the full msg
    np_buffer->fill_buffer(io_socket, msg_size - 4);
    temp = np_buffer->read_nbytes(msg_size - 4);

    // get the message type
    char msg_type = 0;
    msg_type = *temp; // np_buffer->read_byte();

    switch (msg_type) {
    case SP2SP_PUSHLIST:
        get_push_list(temp + 1, msg_size - 5);
        break;
    default:
        printf("RsNpSpProtocol::loop() other msg_type\n");
        break;
    }
    return ret;
}

int RsNpSpProtocol::on_end_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsNpSpProtocol::on_thread_stop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

} // namespace sp
} // namespace protocol
} // namespace rs