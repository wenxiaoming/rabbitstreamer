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
#include "sp_cs_connector.h"
#include "core/error_code.h"
#include "core/logger.h"
#include "core/p2p_protocol.h"
#include "sp_source_manager.h"
#include "third_party/md5/md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace rs {
namespace protocol {
namespace sp {

#define CS_SP_READ_BUFFER_SIZE 65535

//#define DUMP_MEDIA_DATA_FROM_CS

#ifdef DUMP_MEDIA_DATA_FROM_CS
FILE *dump_file = NULL;
static int block_counter = 0;
static bool write_flag = false;
#endif
RsCsSpProtocol::RsCsSpProtocol(st_netfd_t stfd)
    : RsThread("csspprotocol"), st_fd(stfd) {
    io_socket = new RsSocket(st_fd);

    read_buffer = new char[CS_SP_READ_BUFFER_SIZE];
    read_size = CS_SP_READ_BUFFER_SIZE;

    cycle_interval_us = 1 * 1000; // sleep 1ms

    cs_buffer = new RsBuffer;
#ifdef DUMP_MEDIA_DATA_FROM_CS
    dump_file = fopen("capture", "wb");
#endif

    calculator = new RsBitrateCalculator("cs");
}

RsCsSpProtocol::~RsCsSpProtocol() {
    if (read_buffer)
        delete[] read_buffer;
#ifdef DUMP_MEDIA_DATA_FROM_CS
    if (dump_file)
        fclose(dump_file);
#endif
    if (calculator)
        delete calculator;
}

int RsCsSpProtocol::on_thread_start() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::on_before_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::loop() {
    int ret = ERROR_SUCCESS;

    // get data from capture server
    ssize_t ret_size = 0;
    // read 4 bytes to get the message type
    if ((ret = cs_buffer->fill_buffer(io_socket, 4)) != ERROR_SUCCESS) {
        return ret;
    }

    // get message size
    char *temp = cs_buffer->read_nbytes(4);
    int msg_size = 0; //(temp[3]<<24)|(temp[2]<<16)|(temp[1]<<8)|temp[0];
    get_as_type(temp, msg_size);

    // printf("%s msg_size:%d \n", __FUNCTION__, msg_size);

    // read the full msg
    cs_buffer->fill_buffer(io_socket, msg_size - 4);
    temp = cs_buffer->read_nbytes(msg_size - 4);

    // get the message type
    char msg_type = 0;
    get_as_type(temp, msg_type);

    // printf("%s msg_type:%d \n", __FUNCTION__, msg_type);
    switch (msg_type) {
    case CS2SP_REGISTER:
        printf("%s CS2SP_REGISTER\n", __FUNCTION__);
        get_register(temp + 1, msg_size - 5);
        break;
    case CS2SP_UPDATE:
        break;
    case CS2SP_BLOCK:
        // printf("%s CS2SP_BLOCK\n", __FUNCTION__);
        get_block(temp + 1, msg_size - 5);
        break;
    case CS2SP_MEDIA_TYPE:
        printf("%s CS2SP_MEDIA_TYPE\n", __FUNCTION__);
        get_mediatype(temp + 1, msg_size - 5);
        break;
    }

    return ret;
}

int RsCsSpProtocol::get_msg() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::get_mediatype(char *msg, int size) {
    int ret = ERROR_SUCCESS;
    RSLOGI("get_mediatype");
    // get media type's size
    int mediatype_size = 0;
    get_as_type(msg, mediatype_size);
#ifdef DUMP_MEDIA_DATA_FROM_CS
    if (dump_file)
        fwrite(msg + 4, 1, mediatype_size, dump_file);
#endif
    printf("%s mediatype_size:%d \n", __FUNCTION__, mediatype_size);
    RsSourceManager::instance()->add_header(chnl_hash, msg + 4, mediatype_size);
    return ret;
}

int RsCsSpProtocol::get_register(char *msg, int size) {
    int ret = ERROR_SUCCESS;
    RSLOGI("get_register");

    char channel_size = 0;
    get_as_type(msg, channel_size);
    msg += sizeof(channel_size);

    RSLOGI("channel_size:%d ", channel_size);
    char *channel_name = (char *)malloc(channel_size * sizeof(char));
    memcpy(channel_name, msg, channel_size);
    msg += channel_size;

    printf("channel_name:%s channel_size:%d\n", channel_name, channel_size);
    // parse other item
    // userid UINT32
    int user_id = 0;
    get_as_type(msg, user_id);
    msg += sizeof(user_id);

    // generate the hash
    // hash of channel created by this CS
    MD5 md5;
    md5.update(reinterpret_cast<unsigned char *>(channel_name),
               (unsigned int)channel_size);
    md5.finalize();
    char *hash = md5.hex_digest();
    strcpy(chnl_hash.hash_, hash);
    delete[] hash;
    chnl_hash.hash_[MD5_LEN] = 0;

    printf("chnl_hash.hash:%s\n", chnl_hash.hash_);

    char user_password[MD5_LEN + 1];
    memcpy(user_password, msg, MD5_LEN);
    user_password[MD5_LEN] = 0;
    msg += MD5_LEN;

    int block_size = 0;
    get_as_type(msg, block_size);
    msg += sizeof(block_size);

    int file_size = 0;
    get_as_type(msg, file_size);
    msg += sizeof(file_size);

    float bit_rate = 0;
    get_as_type(msg, bit_rate);
    msg += sizeof(bit_rate);

    char is_source = 0;
    get_as_type(msg, is_source);
    msg += sizeof(is_source);

    int header_size = 0;
    get_as_type(msg, header_size);
    msg += sizeof(header_size);

    // add source info to source manager
    RsSourceManager::instance()->create_source(chnl_hash, channel_name, true);
    free(channel_name);

    if (header_size == 0) {
        printf("%s get_register header_size ==0\n", __FUNCTION__);
        RSLOGI("get_register header_size ==0");
        return ret;
    }
    char *header_data = (char *)malloc(header_size * sizeof(char));
    memcpy(header_data, msg, header_size);

#ifdef DUMP_MEDIA_DATA_FROM_CS
    if (dump_file)
        fwrite(header_data, 1, header_size, dump_file);
#endif

    RsSourceManager::instance()->add_header(chnl_hash, header_data,
                                            header_size);

    free(header_data);

    return ret;
}

int RsCsSpProtocol::get_update(char *msg, int size) {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::get_block(char *msg, int size) {
    int ret = ERROR_SUCCESS;
    // block id (UINT32)
    int block_id = 0;
    get_as_type(msg, block_id);
    msg += sizeof(block_id);

    // block size (UINT32)
    int block_size = 0;
    get_as_type(msg, block_size);
    msg += sizeof(block_size);

    // data_offset (UINT32)
    int data_offset = 0;
    get_as_type(msg, data_offset);
    msg += sizeof(data_offset);

    RSLOGI("get_block: id:%d size:%d data_offset:%d ", block_id, block_size,
           data_offset);
    printf("get_block: id:%d size:%d data_offset:%d \n", block_id, block_size,
           data_offset);

    // block data
    char *block_data = NULL;
    RsSourceManager::instance()->queue_block(chnl_hash, &block_data,
                                             block_size);
    memcpy(block_data, msg - 4, block_size);
    // dump to file
#ifdef DUMP_MEDIA_DATA_FROM_CS
    block_counter++;
    if (dump_file && block_counter >= 200 && block_counter <= 10000) {
        if (write_flag == false) {
            if ((block_size - 4 - data_offset) != 0) {
#if 1
                RSLOGI("buffer:0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x",
                       *(block_data + 4 + data_offset),
                       *(block_data + 4 + data_offset + 1),
                       *(block_data + 4 + data_offset + 2),
                       *(block_data + 4 + data_offset + 3),
                       *(block_data + 4 + data_offset + 4),
                       *(block_data + 4 + data_offset + 5),
                       *(block_data + 4 + data_offset + 6),
                       *(block_data + 4 + data_offset + 7));
#endif
                fwrite(block_data + 4 + data_offset, 1,
                       block_size - 4 - data_offset, dump_file);
                write_flag = true;
            }
        } else
            fwrite(block_data + 4, 1, block_size - 4, dump_file);
    }
#endif
    RsSourceManager::instance()->set_block_available(chnl_hash, block_id);
    // free(block_data);
    calculator->update_buffersize(block_size);
    return ret;
}

int RsCsSpProtocol::send_welcome() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::send_err_msg() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::on_end_loop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

int RsCsSpProtocol::on_thread_stop() {
    int ret = ERROR_SUCCESS;

    return ret;
}

} // namespace sp
} // namespace protocol
} // namespace rs