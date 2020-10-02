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

#ifndef PROTOCOL_SP_CS_CONNECTOR_H_
#define PROTOCOL_SP_CS_CONNECTOR_H_

#include "core/bitrate_calculator.h"
#include "core/buffer.h"
#include "core/core_struct.h"
#include "core/socket.h"
#include "core/thread.h"

using namespace rs::core;

namespace rs {
namespace protocol {
namespace sp {

class RsCsSpProtocol : public RsThread {
  public:
    explicit RsCsSpProtocol(st_netfd_t stfd);
    virtual ~RsCsSpProtocol();

  protected:
    int get_msg();
    int get_register(char *msg, int size);
    int get_update(char *msg, int size);
    int get_block(char *msg, int size);
    int get_mediatype(char *msg, int size);

    int send_welcome();
    int send_err_msg();

  public:
    // implement rs_thread's virtual function
    virtual int on_thread_start() override final;
    virtual int on_before_loop() override final;
    virtual int loop() override final;
    virtual int on_end_loop() override final;
    virtual int on_thread_stop() override final;

  private:
    template <class T> int get_as_type(char *buf, T &x) {
        int typesize = sizeof(T);
        memcpy((char *)&x, buf, typesize);
        return 0;
    }

  private:
    MD5_Hash_Str chnl_hash;
    st_netfd_t st_fd = nullptr;
    RsSocket *io_socket = nullptr;
    char *read_buffer = nullptr;
    int read_size = 0;
    RsBuffer *cs_buffer = nullptr;
    RsBitrateCalculator *calculator = nullptr;
};

} // namespace sp
} // namespace protocol
} // namespace rs

#endif /* PROTOCOL_SP_CS_PROTOCOL_H_ */
