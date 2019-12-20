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
#ifndef CORE_LOGGER_H_
#define CORE_LOGGER_H_

#include "error_code.h"

namespace rs {
namespace core {

class RsLogBase {
  public:
    enum LogLevel { Verbose, Info, Trace, Warn, Error, Disabled };

  public:
    RsLogBase();
    virtual ~RsLogBase();

  public:
    virtual int initialize();

  public:
    virtual void log(LogLevel level, const char *tag, int context_id,
                     const char *fmt, ...);
};

// the context for multiple clients.
class RsContextBase {
  public:
    RsContextBase();
    virtual ~RsContextBase();

  public:
    virtual int generate_id();

    virtual int get_id();

    virtual int set_id(int v);
};

void init_log_system();

void deinit_log_system();

RsLogBase *rs_log();
RsContextBase *rs_context();

// donot print method
#if 1
#define RSLOGV(msg, ...)                                                       \
    rs_log()->log(RsLogBase::Verbose, NULL, rs_context()->get_id(), msg,       \
                  ##__VA_ARGS__)
#define RSLOGI(msg, ...)                                                       \
    rs_log()->log(RsLogBase::Info, NULL, rs_context()->get_id(), msg,          \
                  ##__VA_ARGS__)
#define RSLOGT(msg, ...)                                                       \
    rs_log()->log(RsLogBase::Trace, NULL, rs_context()->get_id(), msg,         \
                  ##__VA_ARGS__)
#define RSLOGW(msg, ...)                                                       \
    rs_log()->log(RsLogBase::Warn, NULL, rs_context()->get_id(), msg,          \
                  ##__VA_ARGS__)
#define RSLOGE(msg, ...)                                                       \
    rs_log()->log(RsLogBase::Error, NULL, rs_context()->get_id(), msg,         \
                  ##__VA_ARGS__)
// use __FUNCTION__ to print c method
#elif 0
#define RSLOGV(msg, ...)                                                       \
    rs_log->log(Verbose, __FUNCTION__, rs_context->get_id(), msg, ##__VA_ARGS__)
#define RSLOGI(msg, ...)                                                       \
    rs_log->log(Info, __FUNCTION__, rs_context->get_id(), msg, ##__VA_ARGS__)
#define RSLOGT(msg, ...)                                                       \
    rs_log->log(Trace, __FUNCTION__, rs_context->get_id(), msg, ##__VA_ARGS__)
#define RSLOGW(msg, ...)                                                       \
    rs_log->log(Warn, __FUNCTION__, rs_context->get_id(), msg, ##__VA_ARGS__)
#define RSLOGE(msg, ...)                                                       \
    rs_log->log(Error, __FUNCTION__, rs_context->get_id(), msg, ##__VA_ARGS__)
// use __PRETTY_FUNCTION__ to print c++ class:method
#else
#define RSLOGV(msg, ...)                                                       \
    rs_log->log(Verbose, __PRETTY_FUNCTION__, rs_context->get_id(), msg,       \
                ##__VA_ARGS__)
#define RSLOGI(msg, ...)                                                       \
    rs_log->log(Info, __PRETTY_FUNCTION__, rs_context->get_id(), msg,          \
                ##__VA_ARGS__)
#define RSLOGT(msg, ...)                                                       \
    rs_log->log(Trace, __PRETTY_FUNCTION__, rs_context->get_id(), msg,         \
                ##__VA_ARGS__)
#define RSLOGW(msg, ...)                                                       \
    rs_log->log(Warn, __PRETTY_FUNCTION__, rs_context->get_id(), msg,          \
                ##__VA_ARGS__)
#define RSLOGE(msg, ...)                                                       \
    rs_log->log(Error, __PRETTY_FUNCTION__, rs_context->get_id(), msg,         \
                ##__VA_ARGS__)
#endif

} // namespace core
} // namespace rs

#endif
