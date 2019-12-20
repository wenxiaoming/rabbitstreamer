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
#include "async_logger.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace rs {
namespace core {

RsThreadContext::RsThreadContext() {}

RsThreadContext::~RsThreadContext() {}

int RsThreadContext::generate_id() {
    static int id = 100;

    int gid = id++;
    cache[st_thread_self()] = gid;
    return gid;
}

int RsThreadContext::get_id() { return cache[st_thread_self()]; }

int RsThreadContext::set_id(int v) {
    st_thread_t self = st_thread_self();

    int ov = 0;
    if (cache.find(self) != cache.end()) {
        ov = cache[self];
    }

    cache[self] = v;

    return ov;
}

// the max size of a line of log.
#define LOG_MAX_SIZE 4096
// the tail append to each log.
#define LOG_TAIL '\n'
// reserved for the end of log data, it must be strlen(LOG_TAIL)
#define LOG_TAIL_SIZE 1

#define MAX_PATH 255

RsAsyncLogger::RsAsyncLogger() {
    log_level = Verbose;
    log_data = new char[LOG_MAX_SIZE];

    fd = -1;
    log_to_file_tank = true;
    utc = false;
}

RsAsyncLogger::~RsAsyncLogger() {
    if (log_data)
        delete[] log_data;

    if (fd > 0) {
        ::close(fd);
        fd = -1;
    }
}

int RsAsyncLogger::initialize() {
    int ret = ERROR_SUCCESS;
    return ret;
}

void RsAsyncLogger::log(LogLevel level, const char *tag, int context_id,
                        const char *fmt, ...) {
    string level_name;

    if (log_level > level)
        return;

    switch (level) {
    case Verbose:
        level_name = "verb";
        break;
    case Info:
        level_name = "info";
        break;
    case Trace:
        level_name = "trace";
        break;
    case Warn:
        level_name = "warn";
        break;
    case Error:
        level_name = "error";
        break;
    default:
        level_name = "unknow";
        break;
    }

    int size = 0;
    if (!generate_header(false, tag, context_id, level_name.c_str(), &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(log_data + size, LOG_MAX_SIZE - size, fmt, ap);
    va_end(ap);

    write_log(fd, log_data, size, level);
}

bool RsAsyncLogger::generate_header(bool error, const char *tag, int context_id,
                                    const char *level_name, int *header_size) {
    // clock time
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return false;
    }

    // to calendar time
    struct tm *tm;
    if (utc) {
        if ((tm = gmtime(&tv.tv_sec)) == NULL) {
            return false;
        }
    } else {
        if ((tm = localtime(&tv.tv_sec)) == NULL) {
            return false;
        }
    }

    // write log header
    int log_header_size = -1;

    if (error) {
        if (tag) {
            log_header_size = snprintf(
                log_data, LOG_MAX_SIZE,
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%s][%d][%d][%d] ",
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour,
                tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), level_name,
                tag, getpid(), context_id, errno);
        } else {
            log_header_size = snprintf(
                log_data, LOG_MAX_SIZE,
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d][%d] ",
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour,
                tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), level_name,
                getpid(), context_id, errno);
        }
    } else {
        if (tag) {
            log_header_size = snprintf(
                log_data, LOG_MAX_SIZE,
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%s][%d][%d] ",
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour,
                tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), level_name,
                tag, getpid(), context_id);
        } else {
            log_header_size = snprintf(
                log_data, LOG_MAX_SIZE,
                "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%d] ",
                1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour,
                tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000), level_name,
                getpid(), context_id);
        }
    }

    if (log_header_size == -1) {
        return false;
    }

    // write the header size.
    *header_size = min(LOG_MAX_SIZE - 1, log_header_size);

    return true;
}

void RsAsyncLogger::write_log(int &fd, char *str_log, int size, int level) {
    // ensure the tail and EOF of string
    //      LOG_TAIL_SIZE for the TAIL char.
    //      1 for the last char(0).
    size = min(LOG_MAX_SIZE - 1 - LOG_TAIL_SIZE, size);

    // add some to the end of char.
    str_log[size++] = LOG_TAIL;

    // if not to file, to console and return.
    if (!log_to_file_tank) {
        // if is error msg, then print color msg.
        // \033[31m : red text code in shell
        // \033[32m : green text code in shell
        // \033[33m : yellow text code in shell
        // \033[0m : normal text code
        if (level <= Trace) {
            printf("%.*s", size, str_log);
        } else if (level == Warn) {
            printf("\033[33m%.*s\033[0m", size, str_log);
        } else {
            printf("\033[31m%.*s\033[0m", size, str_log);
        }

        return;
    }

    // open log file. if specified
    if (fd < 0) {
        open_log_file();
    }

    // write log to file.
    if (fd > 0) {
        ::write(fd, str_log, size);
    }
}

void RsAsyncLogger::open_log_file() {
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return;
    }

    struct tm *tm;
    if ((tm = localtime(&tv.tv_sec)) == NULL) {
        return;
    }

    char filename[MAX_PATH];
    snprintf(filename, MAX_PATH, "log-%d-%02d-%02d-%02d-%02d-%02d.%03d\0",
             1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday, tm->tm_hour,
             tm->tm_min, tm->tm_sec, (int)(tv.tv_usec / 1000));

    fd = ::open(filename, O_RDWR | O_APPEND);

    if (fd == -1 && errno == ENOENT) {
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    }
}

static RsLogBase *sRsLog = nullptr;

static RsContextBase *sRscontext = nullptr;

void init_log_system() {
    sRsLog = new RsAsyncLogger();
    sRscontext = new RsThreadContext();
}

void deinit_log_system() {
    if (sRsLog)
        delete sRsLog;

    if (sRscontext)
        delete sRscontext;
}

RsLogBase *rs_log() { return sRsLog; }

RsContextBase *rs_context() { return sRscontext; }

} // namespace core
} // namespace rs