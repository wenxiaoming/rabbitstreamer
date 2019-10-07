/*
 * rs_async_logger.h
 *
 *  Created on: Apr 16, 2017
 *      Author: root
 */

#ifndef CORE_RS_ASYNC_LOGGER_H_
#define CORE_RS_ASYNC_LOGGER_H_

#include <st.h>
#include <map>
#include "logger.h"

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

class RsAsyncLogger: public RsLogBase {
public:
    RsAsyncLogger();
    virtual ~RsAsyncLogger();
public:
    virtual int initialize();
public:
    virtual void log(LogLevel level, const char* tag, int context_id, const char* fmt, ...);
private:
    virtual bool generate_header(bool error, const char* tag, int context_id, const char* level_name, int* header_size);
    virtual void write_log(int& fd, char* str_log, int size, int level);
    virtual void open_log_file();
private:
    int log_level;
    char* log_data;
    int fd;
    bool log_to_file_tank;
    bool utc;
};

} } // namespace rs::core

#endif /* CORE_RS_ASYNC_LOGGER_H_ */
