/*
The MIT License (MIT)

Copyright (c) 2016-2018 RabbitStreamer

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

#ifndef CORE_RS_ASYNC_LOGGER_H_
#define CORE_RS_ASYNC_LOGGER_H_

#include "logger.h"
#include <map>
#include <st.h>

using namespace std;

namespace rs {
namespace core {

class RsThreadContext : public RsContextBase {
  private:
    map<st_thread_t, int> cache;

  public:
    RsThreadContext();
    virtual ~RsThreadContext();

  public:
    virtual int generate_id();
    virtual int get_id();
    virtual int set_id(int v);
};

class RsAsyncLogger : public RsLogBase {
  public:
    RsAsyncLogger();
    virtual ~RsAsyncLogger();

  public:
    virtual int initialize();

  public:
    virtual void log(LogLevel level, const char *tag, int context_id,
                     const char *fmt, ...);

  private:
    virtual bool generate_header(bool error, const char *tag, int context_id,
                                 const char *level_name, int *header_size);
    virtual void write_log(int &fd, char *str_log, int size, int level);
    virtual void open_log_file();

  private:
    int log_level = Verbose;
    char *log_data;
    int fd = -1;
    bool log_to_file_tank = true;
    bool utc = false;
};

} // namespace core
} // namespace rs

#endif /* CORE_RS_ASYNC_LOGGER_H_ */
