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

#ifndef PROTOCOL_SP_NP_PROTOCOL_H_
#define PROTOCOL_SP_NP_PROTOCOL_H_

#include "core/thread.h"
#include "core/socket.h"
#include "core/buffer.h"
#include "core/p2p_protocol.h"
#include "core/bitrate_calculator.h"
#include <st.h>
#include <string.h>
#include <stdint.h>

class RsNpSpProtocol : public RsThread
{
public:
    RsNpSpProtocol(st_netfd_t stfd);
    virtual ~RsNpSpProtocol();
public:
    int get_push_list(char* msg, int size);
    int send_push_list(char* resource_hash, uint8_t count, uint32_t* array);
    int send_media_type(char* resource_hash);
public:
    //implement rs_thread's virtual function
    virtual int on_thread_start();
    virtual int on_before_loop();
    virtual int loop();
    virtual int on_end_loop();
    virtual int on_thread_stop();
private:
    int send_one_block(char* resource_hash, uint8_t count, uint32_t id);
private:
    st_netfd_t st_fd;
    RsSocket* io_socket;
    RsBuffer* np_buffer;
    bool media_type_flag;
    RsBitrateCalculator* calculator;
};

#endif /* PROTOCOL_SP_NP_PROTOCOL_H_ */
